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

#pragma once

#include "core/core.h"
#include "fileutil.h"
#include <map>

// ParamSet Declarations
class ParamSet
{
public:

    ParamSet() {}
    void AddFloat(const std::string& name, std::unique_ptr<float[]> v,
                  int nValues = 1);
    void AddInt(const std::string& name, std::unique_ptr<int[]> v, int nValues);

    bool EraseFloat(const std::string& name);

    void ReportUnused() const;
    void Clear();
    std::string ToString() const;
    void Print(int indent) const;

private:

    friend class TextureParams;

    std::vector<std::shared_ptr<ParamSetItem<int>>> ints;
    std::vector<std::shared_ptr<ParamSetItem<float>>> floats;
};

template <typename T>
struct ParamSetItem
{
    ParamSetItem(const std::string& name, std::unique_ptr<T[]> value,
                 int nValeus = 1);

    const std::string name;
    const std::unique_ptr<T[]> values;
    const int nValues;
    mutable bool lookedUp = false;
};

template <typename T>
ParamSetItem<T>::ParamSetItem(const std::string &name, std::unique_ptr<T[]> v,
                              int nValues)
    : name(name), values(std::move(v)), nValues(nValues) {}
