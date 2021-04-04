// spectral periodogram waterfall object
#ifndef __SPWATERFALL_HH__
#define __SPWATERFALL_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.h"
#include "liquid.python.hh"

namespace liquid {

#pragma GCC visibility push(hidden)
class spwaterfall
{
  public:
    spwaterfall(unsigned int _nfft,
                unsigned int _time,
                unsigned int _window_len,
                unsigned int _delay,
                std::string  _wtype)
        {
            liquid_window_type wtype = liquid_getopt_str2window(_wtype.c_str());
            if (wtype == LIQUID_WINDOW_UNKNOWN)
                throw std::runtime_error("invalid/unknown window type: " + _wtype);
            q = spwaterfallcf_create(_nfft, wtype, _window_len, _delay, _time);
        }

    spwaterfall(unsigned int _nfft,
                unsigned int _time)
        { q = spwaterfallcf_create_default(_nfft, _time); }

    ~spwaterfall() { spwaterfallcf_destroy(q); }

    void display() { spwaterfallcf_print(q); }

    void reset() { spwaterfallcf_reset(q); }

    uint64_t      get_num_samples_total() { return spwaterfallcf_get_num_samples_total(q); }
    unsigned int  get_num_freq() { return spwaterfallcf_get_num_freq(q); }
    unsigned int  get_num_time() { return spwaterfallcf_get_num_time(q); }
    const float * get_psd()      { return spwaterfallcf_get_psd     (q); }

    void execute(std::complex<float> _v)
        { spwaterfallcf_push(q, _v); }

    void execute(std::complex<float> * _buf, unsigned int _buf_len)
        { spwaterfallcf_write(q, _buf, _buf_len); }

  private:
    spwaterfallcf q;

#ifdef PYTHONLIB
  public:
    void py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.itemsize != sizeof(std::complex<float>))
            throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // execute on input
        execute((std::complex<float>*) info.ptr, info.shape[0]);
    }

    py::tuple py_get_psd()
    {
        unsigned int nfreq = get_num_freq();
        unsigned int ntime = get_num_time();
        uint64_t     nsamp = get_num_samples_total();

        // make return arrays
        py::array_t<float> freq(nfreq);
        py::array_t<float> time(ntime);
        py::array_t<float> Sxx({nfreq,ntime},{sizeof(float),nfreq*sizeof(float),},get_psd());

        // populate time, frequency arrays
        float * _freq = (float*) freq.request().ptr;
        float * _time = (float*) time.request().ptr;
        for (auto i=0U; i<nfreq; i++)
            _freq[i] = (float)i / (float)nfreq - 0.5f;
        for (auto i=0U; i<ntime; i++)
            _time[i] = (float)i / (float)ntime * (float)nsamp;

        // 
        return py::make_tuple(Sxx,time,freq);
    }

#endif
};
#pragma GCC visibility pop

#ifdef PYTHONLIB
void init_spwaterfall(py::module &m)
{
    py::class_<spwaterfall>(m, "spwaterfall")
        .def(py::init<unsigned int,unsigned int,unsigned int,unsigned int,std::string>(),
             py::arg("nfft")=800,
             py::arg("time")=800,
             py::arg("wlen")=600,
             py::arg("delay")=400,
             py::arg("wtype")="hamming")
        .def("reset",   &spwaterfall::reset,      "reset waterfall object")
        .def("execute", &spwaterfall::py_execute, "execute on a block of samples")
        .def("get_num_samples_total",
             &spwaterfall::get_num_samples_total,
             "get total number of samples generated")
        .def("get_psd",
             &spwaterfall::py_get_psd,
             "get power spectral density")
        ;
}
#endif

} // namespace liquid

#endif //__SPWATERFALL_HH__