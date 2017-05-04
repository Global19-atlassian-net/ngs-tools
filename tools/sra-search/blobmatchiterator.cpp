/*===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
*/

#include "blobmatchiterator.hpp"

#include <sstream>

#include <kproc/lock.h>

#include <ngs-vdb/inc/NGS-VDB.hpp>

#include "searchbuffer.hpp"

using namespace std;
using namespace ngs;
using namespace ncbi::ngs::vdb;

////////////////////////////////// BlobSearchBuffer

class BlobSearchBuffer : public SearchBuffer
{
public:
    BlobSearchBuffer ( VdbReadCollection    p_coll,
                       SearchBlock*         p_sb,
                       const std::string&   p_accession,
                       KLock*               p_lock,
                       const FragmentBlob&  p_blob )
    :   SearchBuffer ( p_sb, p_accession ),
        m_coll ( p_coll ),
        m_dbLock ( p_lock ),
        m_blob ( p_blob ),
        m_startInBlob ( 0 )
    {
        KLockAddRef ( m_dbLock );
    }

    ~BlobSearchBuffer ()
    {
        KLockRelease ( m_dbLock );
    }

    virtual SearchBuffer :: Match * NextMatch ()
    {
        string id = BufferId();

        uint64_t hitStart;
        uint64_t hitEnd;
        while ( m_searchBlock -> FirstMatch ( m_blob . Data () + m_startInBlob, m_blob . Size () - m_startInBlob, hitStart, hitEnd  ) )
        {
            // convert to offsets from the start of the blob
            hitStart += m_startInBlob;
            hitEnd += m_startInBlob;

            string fragId;
            uint64_t startInBlob;
            uint64_t lengthInBases;
            bool biological;

            KLockAcquire ( m_dbLock );
            try
            {
                m_blob . GetFragmentInfo ( hitStart, fragId, startInBlob, lengthInBases, biological );
            }
            catch ( ... )
            {
                KLockUnlock ( m_dbLock );
                throw;
            }
            KLockUnlock ( m_dbLock );

            uint64_t fragEnd = startInBlob + lengthInBases; // relative to the start of the blob

            if ( biological )
            {
                if ( hitEnd < fragEnd ||                                                                  // inside a fragment: report and move to the next fragment; or
                    m_searchBlock -> FirstMatch ( m_blob . Data () + startInBlob, lengthInBases  ) )    // result crosses fragment boundary: retry within the fragment
                {
                    Match * ret = 0;
                    ret = new Match ( m_accession, fragId, string ( m_blob . Data () + startInBlob, lengthInBases ) );
                    m_startInBlob = fragEnd; // search will resume with the next fragment
                    return ret;
                }
                // false hit
            }
            m_startInBlob = fragEnd; // search will resume with the next fragment
        }
        m_startInBlob = 0;
        return 0;
    }

    virtual std::string BufferId () const
    {   // identify by row Id range
        int64_t first;
        uint64_t count;
        m_blob . GetRowRange ( first, count );
        ostringstream ret;
        ret << first << "-" << ( first + count - 1 );
        return ret.str();
    }

private:
    VdbReadCollection   m_coll;
    KLock*              m_dbLock;
    FragmentBlob        m_blob;
    uint64_t            m_startInBlob;
};

////////////////////////////////// BlobMatchIterator

BlobMatchIterator :: BlobMatchIterator ( SearchBlock :: Factory& p_factory, const std::string& p_accession )
:   MatchIterator ( p_factory, p_accession ),
    m_coll ( NGS_VDB :: openVdbReadCollection ( p_accession ) ),
    m_blobIt ( m_coll . getFragmentBlobs() )
{
    rc_t rc = KLockMake ( & m_accessionLock );
    if ( rc != 0 )
    {
        throw ( ErrorMsg ( "KLockMake failed" ) );
    }
}

BlobMatchIterator :: ~BlobMatchIterator ()
{
    KLockRelease ( m_accessionLock );
}

SearchBuffer*
BlobMatchIterator :: NextBuffer ()
{
    KLockAcquire ( m_accessionLock );
    SearchBuffer* ret = 0;
    if ( m_blobIt . hasMore () )
    {
        ret =  new BlobSearchBuffer ( m_coll, m_factory.MakeSearchBlock(), m_accession, m_accessionLock, m_blobIt . nextBlob () );
    }
    KLockUnlock ( m_accessionLock );
    return ret;
}

