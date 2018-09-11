/*
   pbrt source code is Copyright(c) 1998-2016
   Matt Pharr, Greg Humphreys, and Wenzel Jakob.
   This file is part of pbrt.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
met:
- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "paramset.h"

void ParamSet::AddFloat(const std::string& name,
                        std::unique_ptr<float[]> values, int nValues)
{
    EraseFloat(name);
    floats.emplace_back(
        new ParamSetItem<float>(name, std::move(values), nValues));
}

bool ParamSet::EraseFloat(const std::string& name)
{
    for (size_t i = 0; i < floats.size(); i++)
        ;
    return false;
}

static int print(int i) { return printf("%d ", i); }
static int print(bool v) {
    return v ? printf("\"true\" ") : printf("\"false\" ");
}
static int print(float f) {
    if ((int)f == f)
        return printf("%d ", (int)f);
    else
        return printf("%.9g ", f);
}

template <typename T>
static void printItems(
    const char *type, int indent, 
    const std::vector<std::shared_ptr<ParamSetItem<T>>> &items)
{
    for (const auto &item : items)
    {
        int np = printf("\n%*s\"%s %s\" [ ", indent + 8, "", type,
                        item->name.c_str());
        for (int i = 0; i < item->nValues; ++i)
        {
            np += print(item->values[i]);
            if (np > 80 && i < item->nValues- 1)
                np = printf("\n%*s", indent + 8, "");
        }
        printf("] ");
    }
}

void ParamSet::Print(int indent) const {
    printItems("integer", indent, ints);
    printItems("float", indent, floats);
}
