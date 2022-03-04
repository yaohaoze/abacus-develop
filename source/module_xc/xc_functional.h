//==========================================================
// AUTHOR : mohan
// DATE : 2009-04-04
//==========================================================
#ifndef XC_FUNCTIONAL_H
#define XC_FUNCTIONAL_H

#ifdef USE_LIBXC
#include <xc.h>
#else
#include "xc_funcs.h"
#endif	// ifdef USE_LIBXC

#include "../module_base/global_function.h"
#include "../module_base/global_variable.h"
#include "../module_base/vector3.h"
#include "../module_base/matrix.h"
#include "exx_global.h"
class XC_Functional
{
	public:

	friend class Run_lcao;
	friend class Run_pw;
	friend class Ions;
	friend class ELEC_scf;
	friend class LOOP_elec;
	friend class Run_MD_PW;

	XC_Functional();
	~XC_Functional();

//-------------------
// subroutines, grouped according to the file they are in:
//-------------------

//-------------------
//  xc_functional_vxc.cpp
//-------------------

// This file contains interface to the xc_functional class
// it includes 3 subroutines:
// 1. v_xc : which takes rho as input, and [etxc, vtxc, v_xc] as output
// 2. v_xc_libxc : which does the same thing as v_xc, but calling libxc
// NOTE : it is only used for nspin = 1 and 2, the nspin = 4 case is treated in v_xc
// 3. v_xc_meta : which takes rho and tau as input, and v_xc as output

	public:

	// compute the exchange-correlation energy 
	// [etxc, vtxc, v] = v_xc(...)
    static std::tuple<double,double,ModuleBase::matrix> v_xc(
		const int &nrxx, // number of real-space grid
		const int &ncxyz, // total number of charge grid
		const double &omega, // volume of cell
		const double*const*const rho_in, 
		const double*const rho_core); // core charge density

	// using libxc
    static std::tuple<double,double,ModuleBase::matrix> v_xc_libxc(
		const int &nrxx, // number of real-space grid
		const int &ncxyz, // total number of charge grid
		const double &omega, // volume of cell
		const double*const*const rho_in,  
		const double*const rho_core); // core charge density

	// for mGGA functional
	static std::tuple<double,double,ModuleBase::matrix,ModuleBase::matrix> v_xc_meta(
		const int &nrxx, // number of real-space grid
		const int &ncxyz, // total number of charge grid
		const double &omega, // volume of cell
		const double * const * const rho_in,
		const double * const rho_core_in,
		const double * const * const kin_r_in);

//-------------------
//  xc_functional.cpp
//-------------------

// This file contains subroutines for setting the functional
// it includes 4 subroutines:
// 1. get_func_type : which returns the type of functional (func_type):
//		0 = none; 1 = lda; 2 = gga; 3 = mgga; 4 = hybrid
// 2. set_xc_type : sets the value of:
//		func_id, which is the LIBXC id of functional
//		func_type, which is as specified in get_func_type
//		use_libxc, whether to use LIBXC. The rule is to NOT use it for functionals that we already have.
// 3. set_xc_type_libxc : sets functional type, which allows combination of LIBXC keyword connected by "+"
//		for example, "XC_LDA_X+XC_LDA_C_PZ"
// 4. init_func : which converts func_id into corresponding xc_func_type vector

	public:

	static int get_func_type();

	private:

	static void set_xc_type(const std::string xc_func_in);
#ifdef USE_LIBXC
	static void set_xc_type_libxc(const std::string xc_func_in);
	static std::vector<xc_func_type> init_func(const int xc_polarized);
#endif

	static std::vector<int> func_id; // libxc id of functional
	static int func_type; //0:none, 1:lda, 2:gga, 3:mgga, 4:hybrid
	static bool use_libxc;

//-------------------
//  xc_functional_wrapper_xc.cpp
//-------------------

// This file contains wrapper for the LDA functionals
// it includes 3 subroutines:
// 1. xc, which is the wrapper of LDA part
// (i.e. LDA functional and LDA part of GGA functional)
// 2. xc_spin, which is the spin polarized counterpart of xc
// 3. xc_spin_libxc, which is the wrapper for LDA functional, spin polarized

// NOTE : In our own realization of GGA functional, the LDA part
// and gradient correction are calculated separately.
// The LDA part is provided in xc, while the gradient correction is 
// provided in gradcorr through gcxc/gcx_spin+gcc_spin.
// While in LIBXC, the entire GGA functional is provided.
// As a result, xc/xc_spin and xc_spin_libxc are different for GGA,
// the former gives nonzero result, while the latter returns 0.
// Furthermore, the reason for not having xc_libxc is that something like
// xc_libxc which evaluates functional for individual grid points
// is not efficient. For nspin = 1 and 2, v_xc_libxc evaluates potential
// on the entire grid. I'm having xc_spin_libxc because v_xc_libxc
// does not support nspin = 4.

	public :

	// LDA
	static void xc(const double &rho, double &exc, double &vxc);

	// LSDA
	static void xc_spin(const double &rho, const double &zeta,
			double &exc, double &vxcup, double &vxcdw);
	static void xc_spin_libxc(const double &rhoup, const double &rhodw,
			double &exc, double &vxcup, double &vxcdw);

//-------------------
//  xc_functional_wrapper_gcxc.cpp
//-------------------

// This file contains wrapper for the GGA functionals
// it includes 4 subroutines:
// 1. gcxc, which is the wrapper for gradient correction part
// 2. gcx_spin, spin polarized, exchange only
// 3. gcc_spin, spin polarized, correlation only
// 4. gcxc_spin_libxc, the entire GGA functional, LIBXC

// The difference between our realization (gcxc/gcx_spin/gcc_spin) and
// LIBXC, and the reason for not having gcxc_libxc is explained
// in the NOTE in the comment for xc_functional_wrapper_wc.cpp part

	public:

	// GGA
	static void gcxc(const double &rho, const double &grho,
			double &sxc, double &v1xc, double &v2xc);

	// spin polarized GGA
	static void gcx_spin(double rhoup, double rhodw, double grhoup2, double grhodw2,
            double &sx, double &v1xup, double &v1xdw, double &v2xup,
            double &v2xdw);
	static void gcc_spin(double rho, double &zeta, double grho, double &sc,
            double &v1cup, double &v1cdw, double &v2c);

	static void gcxc_spin_libxc(double rhoup, double rhodw, 
		ModuleBase::Vector3<double> gdr1, ModuleBase::Vector3<double> gdr2,
        double &sxc, double &v1xcup, double &v1xcdw, double &v2xcup, double &v2xcdw, double &v2xcud);

//-------------------
//  xc_functional_wrapper_tauxc.cpp
//-------------------

// This file contains wrapper for the mGGA functionals
// it includes 2 subroutines:
// 1. tau_xc
// 2. tau_xc_spin

// NOTE : mGGA is realized through LIBXC

#ifdef USE_LIBXC
	public:

	struct Mgga_spin_in
	{
		double rhoup, rhodw;//electron densities
		ModuleBase::Vector3<double> grhoup, grhodw;//gradient of electron densities
		double tauup, taudw;//kinetic energy densities
	};

	struct Mgga_spin_out
	{
		double ex, ec;//xc energy densities
		double v1xup, v1xdw;//vx: lda part
		double v2xup, v2xdw;//vx: gga part
		double v3xup, v3xdw;//vx: mgga part
		double v1cup, v1cdw;//vc: lda part
		ModuleBase::Vector3<double> v2cup, v2cdw;
		std::vector<double> v2c;//vc: gga part, two different formats	
		double v3cup, v3cdw;//vc: mgga part
	};
	
	// mGGA
	static void tau_xc(const double &rho, const double &grho, const double &atau, double &sx, double &sc,
          double &v1x, double &v2x, double &v3x, double &v1c, double &v2c, double &v3c);
	static void tau_xc_spin(const Mgga_spin_in &mgga_spin_in, Mgga_spin_out &mgga_spin_out);
#endif 

//-------------------
//  xc_functional_gradcorr.cpp
//-------------------

// This file contains subroutines realted to gradient calculations
// it contains 5 subroutines:
// 1. gradcorr, which calculates gradient correction
// 2. grad_wfc, which calculates gradient of wavefunction
// 

	public:

	static void gradcorr(double &etxc, double &vtxc, ModuleBase::matrix &v, std::vector<double> &stress_gga, const bool is_stress = 0);
	static void grad_wfc( const std::complex<double> *rhog, const int ik, std::complex<double> **grad, const int npw );

	private:
	
	static void grad_rho( const std::complex<double> *rhog, ModuleBase::Vector3<double> *gdr );
	static void grad_dot( const ModuleBase::Vector3<double> *h, double *dh);
	static void noncolin_rho(double *rhoout1,double *rhoout2,double *seg);

	// For LDA exchange energy
	static void slater(const double &rs, double &ex, double &vx);
	static void slater1(const double &rs, double &ex, double &vx);
	static void slater_rxc(const double &rs, double &ex, double &vx);

	// For LSDA exchange energy
	static void slater_spin( const double &rho, const double &zeta,
		double &ex, double &vxup, double &vxdw);
	static void slater1_spin( const double &rho, const double &zeta,
		double &ex, double &vxup, double &vxdw);
	static void slater_rxc_spin( const double &rho, const double &z,
		double &ex, double &vxup, double &vxdw);

	// For LDA correlation energy
	static void pz(const double &rs, const int &iflag, double &ec, double &vc);
	static void vwn(const double &rs, double &ec, double &vc);
	static void pw(const double &rs, const int &iflag, double &ec, double &vc);
	static void lyp(const double &rs, double &ec, double &vc);
	static void wigner(const double &rs, double &ec, double &vc);
	static void hl(const double &rs, double &ec, double &vc);
	static void gl(const double &rs, double &ec, double &vc);

	// For LSDA correlation energy
	// pz and pz_polarized should be put together
	static void pz_spin( const double &rs, const double &zeta,
    	double &ec, double &vcup, double &vcdw);
	static void pz_polarized( const double &rs, double &ec, double &vc);
	static void pw_spin( const double &rs, const double &zeta,
        double &ec, double &vcup, double &vcdw);

	// PBEx, PBEc
	static void pbex(const double &rho, const double &grho, const int &iflag,
		double &sx, double &v1x, double &v2x);

	static void pbec(const double &rho, const double &grho, const int &flag,
		double &sc, double &v1c, double &v2c);

	// some GGA functionals
	static void hcth(const double rho, const double grho, double &sx, double &v1x, double &v2x);
	static void pwcorr(const double r, const double c[], double &g, double &dg);
	static void optx(const double rho, const double grho, double &sx, double &v1x, double &v2x);
	
	// For PW86 correlation functional
	static void perdew86(const double rho, const double grho, double &sc, double &v1c, double &v2c);

	// For PW91
	static void ggax(const double &rho, const double &grho, double &sx, double &v1x, double &v2x);
	static void ggac(const double &rho,const double &grho, double &sc, double &v1c, double &v2c);

	// For Wu-Cohen
	static void wcx(const double &rho,const double &grho, double &sx, double &v1x, double &v2x);

	// For BLYP
	static void becke88(const double &rho, const double &grho, double &sx, double &v1x, double &v2x);
	static void glyp(const double &rho, const double &grho, double &sc, double &v1c, double &v2c);

	// spin-polarized GGA
	static void becke88_spin(double rho, double grho, double &sx, double &v1x,
			double &v2x);
	static void perdew86_spin(double rho, double zeta, double grho, double &sc,
			double &v1cup, double &v1cdw, double &v2c);
	static void ggac_spin(double rho, double zeta, double grho, double &sc,
			double &v1cup, double &v1cdw, double &v2c);
	static void pbec_spin(double rho, double zeta, double grho, const int &flag, double &sc,
			double &v1cup, double &v1cdw, double &v2c);
};

#endif //XC_FUNCTION_H
