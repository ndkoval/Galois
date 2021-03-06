/*
 * This file belongs to the Galois project, a C++ library for exploiting parallelism.
 * The code is being released under the terms of the 3-Clause BSD License (a
 * copy is located in LICENSE.txt at the top-level directory).
 *
 * Copyright (C) 2018, The University of Texas at Austin. All rights reserved.
 * UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
 * SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR ANY PARTICULAR PURPOSE, NON-INFRINGEMENT AND WARRANTIES OF
 * PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE FROM COURSE OF
 * DEALING OR USAGE OF TRADE.  NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH
 * RESPECT TO THE USE OF THE SOFTWARE OR DOCUMENTATION. Under no circumstances
 * shall University be liable for incidental, special, indirect, direct or
 * consequential damages or loss of profits, interruption of business, or
 * related expenses which may arise from use of Software or Documentation,
 * including but not limited to those resulting from defects in Software and/or
 * Documentation, or loss or inaccuracy of data of any kind.
 */

#include <iostream>
#include <cstring>

#include "galois/runtime/NetworkBackend.h"

using namespace galois::runtime;

int main(int argc, char** argv) {
  NetworkBackend& net = getSystemNetworkBackend();

  for (int i = 1; i < net.Num(); ++i) {
    auto* sb = net.allocSendBlock();
    sb->size = net.size();
    sb->dest = (net.ID() + i) % net.Num();
    if (net.ID() == 0)
      std::strcpy((char*)sb->data, "Hi there minions");
    else
      std::strcpy((char*)sb->data, "Hello");
    net.send(sb);
  }
  int seen = 1;
  while (seen < net.Num()) {
    NetworkBackend::SendBlock* rb = nullptr;
    while (!rb)
      rb = net.recv();

    std::cout << rb->dest << ":" << rb->size << "|" << rb->data << "| at "
              << net.ID() << "\n";
    net.freeSendBlock(rb);
    ++seen;
  }

  return 0;
}
