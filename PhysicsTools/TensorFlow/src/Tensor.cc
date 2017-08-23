/*
 * TensorFlow tensor interface.
 * Based on TensorFlow C API 1.1.
 * For more info, see https://gitlab.cern.ch/mrieger/CMSSW-DNN.
 *
 * Author: Marcel Rieger
 */

#include "PhysicsTools/TensorFlow/interface/Tensor.h"

namespace tf
{

Tensor::Tensor()
    : tf_tensor(0)
    , prod(0)
{
}

Tensor::Tensor(int rank, Shape* shape, DataType dtype)
    : tf_tensor(0)
    , prod(0)
{
    init(rank, shape, dtype);
}

Tensor::~Tensor()
{
    reset();
}

size_t Tensor::getTensorSize(int rank, Shape* shape, DataType dtype)
{
    // get the byte size of one element of that dtype
    size_t elementSize = TF_DataTypeSize(dtype);

    // get the number of elements
    size_t nElements = 1;
    for (int i = 0; i < rank; ++i)
    {
        nElements *= shape[i];
    }

    return nElements * elementSize;
}

void Tensor::init(TF_Tensor* t)
{
    reset();

    // simply assign the tensor
    tf_tensor = t;

    // store cumulative products of the axis dimensions that are used for indexing
    // see getIndex for more info
    int rank = getRank();
    prod = new int64_t[rank];
    for (int i = rank - 1; i >= 0; i--)
    {
        if (i == rank - 1)
        {
            prod[i] = 1;
        }
        else
        {
            prod[i] = getShape(i+1) * prod[i+1];
        }
    }
}

void Tensor::reset()
{
    // delete the tensorflow tensor object
    if (tf_tensor)
    {
        TF_DeleteTensor(tf_tensor);
        tf_tensor = 0;
    }

    // delete cache shape products
    if (prod)
    {
        delete[] prod;
        prod = 0;
    }
}

int Tensor::getAxis(int axis) const
{
    if (empty())
    {
        return -1;
    }

    const int rank = getRank();

    // special treatment of scalars
    if (rank == 0)
    {
        return -1;
    }

    // wrap the axis once, e.g. for rank 3, axis -1 will be 2
    int wrappedAxis = axis;
    if (wrappedAxis < 0)
    {
        wrappedAxis += rank;
    }

    // sanity check
    if (wrappedAxis < 0 || wrappedAxis >= rank)
    {
        throw std::runtime_error("axis " + std::to_string(axis) + " invalid for rank "
            + std::to_string(rank));
    }

    return wrappedAxis;
}

Shape Tensor::getIndex(Shape* pos) const
{
    // when the rank and shape of a tensor is known, the function that maps a nD coordinate in the
    // tensor's space to a 1D index representing the memory position is:
    // pos * prod (where both pos and prod are vectors/arrays)
    // prod is cached to increase performance and calculated via:
    // prod_i = { shape_{i+1} * prod_{i+1} , 0 <= i < rank - 1
    //          { 1                        , i = rank - 1

    if (empty())
    {
        throw std::runtime_error("cannot find index on uninitialized tensors");
    }

    Shape index = 0;
    for (int i = 0; i < getRank(); i++)
    {
        index += pos[i] * prod[i];
    }

    return index;
}

} // namespace tf
