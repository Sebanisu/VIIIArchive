#ifndef OPENVIII_LZSS_H
#define OPENVIII_LZSS_H

#include <cstring>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

namespace OpenVIII::Compression {


struct LZSS
{
private:
  constexpr static const unsigned int flagsMask = 0x100U;
  constexpr static const unsigned int flagsBits = 0xFF00U;
  constexpr static const unsigned int offsetMask = 0xF0U;
  constexpr static const unsigned int countMask = 0x0FU;

  constexpr static const unsigned int NotUsed = 4096U;

  constexpr static const auto F = 18U;
  constexpr static const auto N = NotUsed;
  constexpr static const auto NMinus1 = N - 1U;
  constexpr static const auto THRESHOLD = 2U;


  constexpr static const int NPlus1 = N + 1U;
  constexpr static const int NPlus17 = N + 17U;

public:
  // NEW LZSS
  // LZSS.C -- A Data Compression Program
  // (tab = 4 spaces)
  // 4/6/1989 Haruhiko Okumura
  // Use, distribute, and modify this program freely.
  // Please send me your improved versions.
  // PC-VAN SCIENCE
  // NIFTY-Serve PAF01022
  // CompuServe  74050,1022
  // Code borrowed from Java's implementation of LZSS by antiquechrono
  // ClangTidy doesn't like the signed bitwise. says bitwise ops should be unsigned i guess.
  // I updated the code to unsigned where anything used a bit op
  // If you know the dstSize you can reserve that space. If not we'll start with an empty vector.
  // Info at https://github.com/niemasd/PyFF7/wiki/LZSS-Format
  // https://github.com/niemasd/PyFF7/blob/master/PyFF7/lzss.py
  // http://wiki.ffrtt.ru/index.php?title=FF7/LZSS_format


  [[nodiscard]] static auto Decompress(const std::vector<char> &src, size_t dstSize = 0)
  {
    auto dst = std::vector<char>();
    dst.reserve(dstSize);
    auto iterator = src.begin();
    const auto &srcEnd = src.end();
    unsigned int current{ 0 };
    auto textBuf = std::array<unsigned int, NMinus1 + F>();
    // ring buffer of size N, with extra F-1 bytes to facilitate string comparison

    auto r = N - F;
    auto flags = 0U;
    const auto testAtEnd = [&iterator, &srcEnd]() { return iterator + 1 > srcEnd; };
    while (iterator < srcEnd /*&& (dstSize == 0 || dst.size() < dstSize)*/) {
      if (((flags >>= 1U) & flagsMask) == 0) {
        if (testAtEnd()) { break; }
        flags = static_cast<unsigned char>(*iterator++) | flagsBits;// uses higher byte cleverly to Count eight
      }
      if ((flags & 1U) == 1) {// raw value
        if (testAtEnd()) { break; }
        current = static_cast<unsigned char>(*iterator++);
        // if (dstSize != 0 && dst.size() + 1 >= dstSize) break;
        dst.push_back(static_cast<char>(current));
        textBuf.at(r++) = current;
        r &= NMinus1;
      } else {// value previously read
        // get bounds of ring buffer
        if (testAtEnd()) { break; }
        decltype(current) offset = static_cast<unsigned char>(*iterator++);
        if (testAtEnd()) { break; }
        decltype(current) count = static_cast<unsigned char>(*iterator++);
        offset |= ((count & offsetMask) << 4U);
        count = (count & countMask) + THRESHOLD;
        // read from ring buffer
        for (decltype(current) k = 0; k <= count; k++) {
          // get value
          current = textBuf.at((offset + k) & NMinus1);
          // assign value
          // if (dstSize != 0 && dst.size() + 1 >= dstSize) return dst;
          dst.push_back(static_cast<char>(current));
          textBuf.at(r++) = current;
          r &= NMinus1;
        }
      }
    }
    return dst;
  }
  // todo add compression method

  /****************************************************************************
   ** Deling Final Fantasy VIII Field Editor
   ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
   **
   ** This program is free software: you can redistribute it and/or modify
   ** it under the terms of the GNU General Public License as published by
   ** the Free Software Foundation, either version 3 of the License, or
   ** (at your option) any later version.
   **
   ** This program is distributed in the hope that it will be useful,
   ** but WITHOUT ANY WARRANTY; without even the implied warranty of
   ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   ** GNU General Public License for more details.
   **
   ** You should have received a copy of the GNU General Public License
   ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ****************************************************************************/
  /***************************************************************
          4/6/1989 Haruhiko Okumura
          Use, distribute, and modify this program freely.
          Please send me your improved versions.
                  PC-VAN		SCIENCE
                  NIFTY-Serve	PAF01022
                  CompuServe	74050,1022
  *************************************************************
   * Porting to STL - sebanisu.
   * */


  [[nodiscard]] [[maybe_unused]] static auto Compress(const std::vector<char> &src)
  {

    unsigned int match_length = {};
    unsigned int match_position{};

    auto lson = std::array<unsigned int, NPlus1>();
    // left & right children & parents -- These constitute binary search trees.
    auto rson = std::array<unsigned int, 4353>();
    auto dad = std::array<unsigned int, NPlus1>();

    auto text_buf =
      std::array<unsigned char, NPlus17>();// ring buffer of size N, with extra 17 bytes to facilitate string comparison
    unsigned int len;
    unsigned int r;
    unsigned int s;
    unsigned int last_match_length;
    unsigned int i;
    unsigned int c;
    unsigned int code_buf_ptr;
    int curResult{};
    size_t sizeAlloc = src.size() / 2U;
    auto code_buf = std::array<unsigned char, 17>();
    unsigned char mask;
    const auto dataEnd = src.end();
    auto data = src.begin();

    auto result = std::vector<char>();
    result.reserve(sizeAlloc);
    const auto InsertNode = [&text_buf, &rson, &lson, &match_length, &dad, &match_position](const auto &item) {
      /* Inserts string of length 18, text_buf[item..item+18-1], into one of the trees (text_buf[item]'th tree) and
       * returns the longest-match position and length via the global variables match_position and match_length. If
       * match_length = 18, then removes the old node in favor of the new one, because the old one will be deleted
       * sooner. Note item plays double role, as tree node and position in buffer. */

      unsigned int nodeIndex, p, cmp;
      unsigned char *key;

      cmp = 1;
      key = &text_buf[item];
      p = 4097 + key[0];

      rson[item] = lson[item] = NotUsed;
      match_length = 0;

      while (true) {
        if (cmp >= 0) {
          if (rson[p] != NotUsed) {
            p = rson[p];
          } else {
            rson[p] = item;
            dad[item] = p;
            return;
          }
        } else {
          if (lson[p] != NotUsed) {
            p = lson[p];
          } else {
            lson[p] = item;
            dad[item] = p;
            return;
          }
        }

        for (nodeIndex = 1; nodeIndex < 18; nodeIndex++) {
          if ((cmp = key[nodeIndex] - text_buf[p + nodeIndex]) != 0) break;
        }

        if (nodeIndex > match_length) {
          match_position = p;
          if ((match_length = nodeIndex) >= 18) break;
        }
      }

      dad[item] = dad[p];
      lson[item] = lson[p];
      rson[item] = rson[p];

      dad[lson[p]] = item;
      dad[rson[p]] = item;

      if (rson[dad[p]] == p) {
        rson[dad[p]] = item;
      } else {
        lson[dad[p]] = item;
      }
      dad[p] = NotUsed;// remove p
    };
    // deletes node p from tree
    const auto DeleteNode = [&dad, &rson, &lson](auto p) {
      unsigned int q;
      if (dad[p] == NotUsed) return;// not in tree

      if (rson[p] == NotUsed) {
        q = lson[p];
      } else if (lson[p] == NotUsed) {
        q = rson[p];
      } else {
        q = lson[p];
        if (rson[q] != NotUsed) {
          do {
            q = rson[q];
          } while (rson[q] != NotUsed);

          rson[dad[q]] = lson[q];
          dad[lson[q]] = dad[q];
          lson[q] = lson[p];
          dad[lson[p]] = q;
        }
        rson[q] = rson[p];
        dad[rson[p]] = q;
      }
      dad[q] = dad[p];

      if (rson[dad[p]] == p) {
        rson[dad[p]] = q;
      } else {
        lson[dad[p]] = q;
      }
      dad[p] = NotUsed;
    };

    //    if(result.size() < sizeAlloc) {
    //      result.resize(sizeAlloc);
    //    }

    /* quint32
            textsize = 0,//text size counter
            codesize = 0;//code size counter */

    // initialize trees
    /* For i = 0 to NMinus1, rson[i] and lson[i] will be the right and left children of node i. These nodes need not be
    initialized. Also, dad[i] is the parent of node i. These are initialized to NotUsed which stands for 'not used.'
    For i = 0 to 255, rson[4097 + i] is the root of the tree for strings that begin with character i. These are
    initialized to NotUsed. Note there are 256 trees. */

    // for(i=4097 ; i<=4352 ; ++i)	rson[i] = NotUsed;
    // for(i=0 ; i<N ; ++i)	dad[i] = NotUsed;
    dad.fill(NotUsed);
    std::fill(rson.begin() + NPlus1, rson.end(), NotUsed);

    code_buf[0] = 0;// code_buf[1..16] saves eight units of code, and code_buf[0] works as eight flags, "1" representing
                    // that the unit is an unencoded letter (1 byte), "0" a position-and-length pair (2 bytes). Thus,
                    // eight units require at most 16 bytes of code.

    code_buf_ptr = mask = 1;

    s = 0;
    r = 4078;

    //	for(i=s ; i<r ; ++i)
    //		text_buf[i] = '\x0';//Clear the buffer with  any character that will appear often.
    // memset(text_buf, 0, r); //std::array should init with 0s.

    for (len = 0; len < 18 && data < dataEnd; ++len)
      text_buf[r + len] = static_cast<unsigned char>(*data++);// Read 18 bytes into the last 18 bytes of the buffer
    if (/* (textsize =  */ len /* ) */ == 0) {
      result.clear();
      return result;// text of size zero
    }

    for (i = 1; i <= 18; ++i)
      InsertNode(
        r - i);// Insert the 18 strings, each of which begins with one or more 'space' characters.  Note the order in
               // which these strings are inserted.  This way, degenerate trees will be less likely to occur.

    InsertNode(
      r);// Finally, insert the whole string just read.  The global variables match_length and match_position are set.

    do {
      if (match_length > len) match_length = len;// match_length may be spuriously long near the end of text.

      if (match_length <= 2) {
        match_length = 1;// Not long enough match.  Send one byte.
        code_buf[0] |= mask;//'send one byte' flag
        code_buf[code_buf_ptr++] = text_buf[r];// Send unencoded.
      } else {
        code_buf[code_buf_ptr++] = static_cast<unsigned char>(match_position);
        code_buf[code_buf_ptr++] = static_cast<unsigned char>(
          (((match_position >> 4) & 0xF0U))
          | (match_length - (2 + 1)));// Send position and length pair. Note match_length > 2.
      }

      if ((mask = static_cast<unsigned char>((mask << 1U))) == 0)// Shift mask left one bit.
      {
        //			for(i=0 ; i<code_buf_ptr ; ++i)//Send at most 8 units of
        //				result.append(code_buf[i]);//code together
        // result.replace(curResult, code_buf_ptr, (char *)code_buf, code_buf_ptr);
        // pos,len,after,after_len if length the same then would be a memcpy. or .insert()
        result.insert(result.begin() + curResult, code_buf.begin(), code_buf.begin() + code_buf_ptr);
        curResult += code_buf_ptr;
        code_buf[0] = 0;
        code_buf_ptr = mask = 1;
      }

      last_match_length = match_length;
      for (i = 0; i < last_match_length && data < dataEnd; ++i) {
        c = static_cast<unsigned int>(*data++);
        DeleteNode(s);// Delete old strings and
        text_buf[s] = static_cast<unsigned char>(c);// read new bytes

        if (s < 17)
          text_buf[s + N] = static_cast<unsigned char>(
            c);// If the position is near the end of buffer, extend the buffer to make string comparison easier.

        s = (s + 1) & (NMinus1);
        r = (r + 1) & (NMinus1);
        // Since this is a ring buffer, increment the position modulo N.
        InsertNode(r);// Register the string in text_buf[r..r+18-1]
      }

      while (i++ < last_match_length)// After the end of text,
      {
        DeleteNode(s);// no need to read, but
        s = (s + 1) & (NMinus1);
        r = (r + 1) & (NMinus1);
        if (--len) InsertNode(r);// buffer may not be empty.
      }

    } while (len > 0);// until length of string to be processed is zero

    if (code_buf_ptr > 1)// Send remaining code.
    {
      //		for(i = 0; i < code_buf_ptr ; ++i)
      //			result.append(code_buf[i]);
      // result.replace(curResult, code_buf_ptr, (char *)code_buf, code_buf_ptr);
      // pos,len,after,after_len if length the same then would be a memcpy. or .insert()
      result.insert(result.begin() + curResult, code_buf.begin(), code_buf.begin() + code_buf_ptr);
      curResult += code_buf_ptr;
    }
    // result.truncate(curResult);
    result.erase(result.begin() + curResult, result.end());
    return result;
  }

  [[maybe_unused]] static auto Test(const size_t &size)
  {
    if (size <= 0) return true;
    std::vector<char> vecOfRandomNums = std::vector<char>(static_cast<unsigned int>(size));
    if (vecOfRandomNums.empty()) return true;
    std::generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []() {
      return static_cast<char>(static_cast<unsigned int>(rand()) % 255U);
    });
    auto compressed = Compress(vecOfRandomNums);
    auto uncompressed = Decompress(compressed);
    if (std::equal(vecOfRandomNums.begin(), vecOfRandomNums.end(), uncompressed.begin())) {
      std::cout << "Successful compress and uncompress! " << size << " bytes\n";
      return true;
    } else {
      std::cerr << "Failure!\n";
      return false;
    }
  }
};
}// namespace OpenVIII::Compression
#endif