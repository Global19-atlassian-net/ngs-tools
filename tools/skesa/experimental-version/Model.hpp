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

#ifndef _Model_
#define _Model_

#include <array>
#include <algorithm>
#include <utility>

using namespace std;
namespace DeBruijn {

    inline char Complement(char c)
    {
        switch(toupper(c)) {
        case 'A' : return 'T';
        case 'C' : return 'G'; 
        case 'G' : return 'C'; 
        case 'T' : return 'A'; 
        case 'K' : return 'M';
        case 'M' : return 'K';
        case 'R' : return 'Y';
        case 'Y' : return 'R';
        case 'D' : return 'H';
        case 'V' : return 'B';
        case 'H' : return 'D';
        case 'B' : return 'V';
        case 'N' : return 'N';
        default : return c; }
    }

    map<string,char> ToAmbiguousIUPAC = {{"A",'A'}, {"C",'C'}, {"G",'G'}, {"T",'T'}, {"CT",'Y'}, {"AG",'R'}, {"AT",'W'}, {"CG",'S'}, {"GT",'K'}, {"AC",'M'}, {"AGT",'D'}, {"ACG",'V'}, {"ACT",'H'}, {"CGT",'B'}, {"ACGT",'N'}};
    map<char,string> FromAmbiguousIUPAC = {{'A',"A"}, {'C',"C"}, {'G',"G"}, {'T',"T"}, {'Y',"CT"}, {'R',"AG"}, {'W',"AT"}, {'S',"CG"}, {'K',"GT"}, {'M',"AC"}, {'D',"AGT"}, {'V',"ACG"}, {'H',"ACT"}, {'B',"CGT"}, {'N',"ACGT"}};
    string AmbiguousString(string s) {
        sort(s.begin(), s.end());
        s.erase(unique(s.begin(),s.end()), s.end());
        return s;
    }
    bool MatchWithAmbiguousDNA(char a, char b) {
        string aa = FromAmbiguousIUPAC[a];
        string bb = FromAmbiguousIUPAC[b];
        return (aa.find(bb) != string::npos) || (bb.find(aa) != string::npos);
    }

    template <class BidirectionalIterator>
    void ReverseComplementSeq(const BidirectionalIterator& first, const BidirectionalIterator& last)
    {
        for (BidirectionalIterator i( first ); i != last; ++i)
            *i = Complement(*i);
        reverse(first, last);
    }


    //complement of one NT  
    array<const char, 4> comp_NT = {  2,3,0,1  };
    array<const char, 4> bin2NT = {'A','C','T','G'};
    array<const char, 4> binrev = {2,3,0,1};

    //reverse complement of 4NT,  ie one byte   
    array<const unsigned char, 256> revcomp_4NT = {
        0xaa,
        0xea,
        0x2a,
        0x6a,
        0xba,
        0xfa,
        0x3a,
        0x7a,
        0x8a,
        0xca,
        0xa,
        0x4a,
        0x9a,
        0xda,
        0x1a,
        0x5a,
        0xae,
        0xee,
        0x2e,
        0x6e,
        0xbe,
        0xfe,
        0x3e,
        0x7e,
        0x8e,
        0xce,
        0xe,
        0x4e,
        0x9e,
        0xde,
        0x1e,
        0x5e,
        0xa2,
        0xe2,
        0x22,
        0x62,
        0xb2,
        0xf2,
        0x32,
        0x72,
        0x82,
        0xc2,
        0x2,
        0x42,
        0x92,
        0xd2,
        0x12,
        0x52,
        0xa6,
        0xe6,
        0x26,
        0x66,
        0xb6,
        0xf6,
        0x36,
        0x76,
        0x86,
        0xc6,
        0x6,
        0x46,
        0x96,
        0xd6,
        0x16,
        0x56,
        0xab,
        0xeb,
        0x2b,
        0x6b,
        0xbb,
        0xfb,
        0x3b,
        0x7b,
        0x8b,
        0xcb,
        0xb,
        0x4b,
        0x9b,
        0xdb,
        0x1b,
        0x5b,
        0xaf,
        0xef,
        0x2f,
        0x6f,
        0xbf,
        0xff,
        0x3f,
        0x7f,
        0x8f,
        0xcf,
        0xf,
        0x4f,
        0x9f,
        0xdf,
        0x1f,
        0x5f,
        0xa3,
        0xe3,
        0x23,
        0x63,
        0xb3,
        0xf3,
        0x33,
        0x73,
        0x83,
        0xc3,
        0x3,
        0x43,
        0x93,
        0xd3,
        0x13,
        0x53,
        0xa7,
        0xe7,
        0x27,
        0x67,
        0xb7,
        0xf7,
        0x37,
        0x77,
        0x87,
        0xc7,
        0x7,
        0x47,
        0x97,
        0xd7,
        0x17,
        0x57,
        0xa8,
        0xe8,
        0x28,
        0x68,
        0xb8,
        0xf8,
        0x38,
        0x78,
        0x88,
        0xc8,
        0x8,
        0x48,
        0x98,
        0xd8,
        0x18,
        0x58,
        0xac,
        0xec,
        0x2c,
        0x6c,
        0xbc,
        0xfc,
        0x3c,
        0x7c,
        0x8c,
        0xcc,
        0xc,
        0x4c,
        0x9c,
        0xdc,
        0x1c,
        0x5c,
        0xa0,
        0xe0,
        0x20,
        0x60,
        0xb0,
        0xf0,
        0x30,
        0x70,
        0x80,
        0xc0,
        0x0,
        0x40,
        0x90,
        0xd0,
        0x10,
        0x50,
        0xa4,
        0xe4,
        0x24,
        0x64,
        0xb4,
        0xf4,
        0x34,
        0x74,
        0x84,
        0xc4,
        0x4,
        0x44,
        0x94,
        0xd4,
        0x14,
        0x54,
        0xa9,
        0xe9,
        0x29,
        0x69,
        0xb9,
        0xf9,
        0x39,
        0x79,
        0x89,
        0xc9,
        0x9,
        0x49,
        0x99,
        0xd9,
        0x19,
        0x59,
        0xad,
        0xed,
        0x2d,
        0x6d,
        0xbd,
        0xfd,
        0x3d,
        0x7d,
        0x8d,
        0xcd,
        0xd,
        0x4d,
        0x9d,
        0xdd,
        0x1d,
        0x5d,
        0xa1,
        0xe1,
        0x21,
        0x61,
        0xb1,
        0xf1,
        0x31,
        0x71,
        0x81,
        0xc1,
        0x1,
        0x41,
        0x91,
        0xd1,
        0x11,
        0x51,
        0xa5,
        0xe5,
        0x25,
        0x65,
        0xb5,
        0xf5,
        0x35,
        0x75,
        0x85,
        0xc5,
        0x5,
        0x45,
        0x95,
        0xd5,
        0x15,
        0x55
    };


}; // namespace 
#endif /* _Model_ */