/*
 * LSST Data Management System
 *
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 * See the COPYRIGHT file
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the LSST License Statement and
 * the GNU General Public License along with this program.  If not,
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */
#include <memory>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Eigen/Core"
#include "numpy/arrayobject.h"
#include "ndarray/pybind11.h"

#include "lsst/afw/math/SpatialCell.h"
#include "lsst/ip/diffim/KernelCandidate.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace ip {
namespace diffim {

namespace {

/**
 * Wrap `KernelCandidate` and factory function `makeKernelCandidate` for one pixel type
 *
 * @tparam PixelT  Pixel type of image plane of MaskedImage, e.g. `float`
 * @param mod  pybind11 module
 * @param[in] suffix  Class name suffix associated with PixeT, e.g. "F" for `float`
 */
template <typename PixelT>
void declareKernelCandidate(py::module &mod, std::string const &suffix) {
    py::class_<KernelCandidate<PixelT>, std::shared_ptr<KernelCandidate<PixelT>>,
               afw::math::SpatialCellImageCandidate>
            cls(mod, ("KernelCandidate" + suffix).c_str());

    using CandidateSwitch = typename KernelCandidate<PixelT>::CandidateSwitch;

    py::enum_<CandidateSwitch>(cls, "CandidateSwitch")
            .value("ORIG", CandidateSwitch::ORIG)
            .value("PCA", CandidateSwitch::PCA)
            .value("RECENT", CandidateSwitch::RECENT)
            .export_values();

    cls.def(py::init<float const, float const, std::shared_ptr<afw::image::MaskedImage<PixelT>> const &,
                     std::shared_ptr<afw::image::MaskedImage<PixelT>> const &, pex::policy::Policy const &>(),
            "xCenter"_a, "yCenter"_a, "templateMaskedImage"_a, "scienceMaskedImage"_a, "policy"_a);
    cls.def(py::init<std::shared_ptr<afw::table::SourceRecord> const &,
                     std::shared_ptr<afw::image::MaskedImage<PixelT>> const &,
                     std::shared_ptr<afw::image::MaskedImage<PixelT>> const &, pex::policy::Policy const &>(),
            "source"_a, "templateMaskedImage"_a, "scienceMaskedImage"_a, "policy"_a);

    cls.def("getCandidateRating", &KernelCandidate<PixelT>::getCandidateRating);
    cls.def("getSource", &KernelCandidate<PixelT>::getSource);
    cls.def("getTemplateMaskedImage", &KernelCandidate<PixelT>::getTemplateMaskedImage);
    cls.def("getScienceMaskedImage", &KernelCandidate<PixelT>::getScienceMaskedImage);
    cls.def("getKernel", &KernelCandidate<PixelT>::getKernel, "cand"_a);
    cls.def("getBackground", &KernelCandidate<PixelT>::getBackground, "cand"_a);
    cls.def("getKsum", &KernelCandidate<PixelT>::getKsum, "cand"_a);
    cls.def("getKernelImage", &KernelCandidate<PixelT>::getKernelImage, "cand"_a);
    cls.def("getImage", &KernelCandidate<PixelT>::getImage);
    cls.def("getKernelSolution", &KernelCandidate<PixelT>::getKernelSolution, "cand"_a);
    cls.def("getDifferenceImage",
            (afw::image::MaskedImage<PixelT> (KernelCandidate<PixelT>::*)(CandidateSwitch)) &
                    KernelCandidate<PixelT>::getDifferenceImage,
            "cand"_a);
    cls.def("getDifferenceImage", (afw::image::MaskedImage<PixelT> (KernelCandidate<PixelT>::*)(
                                          std::shared_ptr<afw::math::Kernel>, double)) &
                                          KernelCandidate<PixelT>::getDifferenceImage,
            "kernel"_a, "background"_a);
    cls.def("isInitialized", &KernelCandidate<PixelT>::isInitialized);
    cls.def("build", (void (KernelCandidate<PixelT>::*)(afw::math::KernelList const &)) &
                             KernelCandidate<PixelT>::build,
            "basisList"_a);
    cls.def("build",
            (void (KernelCandidate<PixelT>::*)(afw::math::KernelList const &, Eigen::MatrixXd const &)) &
                    KernelCandidate<PixelT>::build,
            "basisList"_a, "hMat"_a);
    mod.def("makeKernelCandidate",
            (std::shared_ptr<KernelCandidate<PixelT>>(*)(
                    float const, float const, std::shared_ptr<afw::image::MaskedImage<PixelT>> const &,
                    std::shared_ptr<afw::image::MaskedImage<PixelT>> const &, pex::policy::Policy const &)) &
                    makeKernelCandidate,
            "xCenter"_a, "yCenter"_a, "templateMaskedImage"_a, "scienceMaskedImage"_a, "policy"_a);
    mod.def("makeKernelCandidate",
            (std::shared_ptr<KernelCandidate<PixelT>>(*)(
                    std::shared_ptr<afw::table::SourceRecord> const &,
                    std::shared_ptr<afw::image::MaskedImage<PixelT>> const &,
                    std::shared_ptr<afw::image::MaskedImage<PixelT>> const &, pex::policy::Policy const &)) &
                    makeKernelCandidate,
            "source"_a, "templateMaskedImage"_a, "scienceMaskedImage"_a, "policy"_a);
}

}  // namespace lsst::ip::diffim::<anonymous>

PYBIND11_PLUGIN(_kernelCandidate) {
    py::module mod("_kernelCandidate", "Python wrapper for KernelCandidate.h");

    // Need to import numpy for ndarray and eigen conversions
    if (_import_array() < 0) {
        PyErr_SetString(PyExc_ImportError, "numpy.core.multiarray failed to import");
        return nullptr;
    }

    declareKernelCandidate<float>(mod, "F");

    return mod.ptr();
}

}  // diffim
}  // ip
}  // lsst