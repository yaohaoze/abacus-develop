#include "ions_move_methods.h"

#include "ions_move_basic.h"
#include "module_hamilt_pw/hamilt_pwdft/global.h"

Ions_Move_Methods::Ions_Move_Methods()
{
}
Ions_Move_Methods::~Ions_Move_Methods()
{
}

void Ions_Move_Methods::allocate()
{
    Ions_Move_Basic::dim = GlobalC::ucell.nat * 3;

    if (GlobalV::RELAX_METHOD == "bfgs")
    {
        this->bfgs.allocate();
    }
    else if (GlobalV::RELAX_METHOD == "sd")
    {
        this->sd.allocate();
    }
    else if (GlobalV::RELAX_METHOD == "cg")
    {
        this->cg.allocate();
    }
    else if (GlobalV::RELAX_METHOD == "cg_bfgs")
    {
        this->cg.allocate();
        this->bfgs.allocate(); // added by pengfei  13-8-8
    }
    else
    {
        ModuleBase::WARNING("Ions_Move_Methods::init", "the parameter GlobalV::RELAX_METHOD is not correct.");
    }
    return;
}

// void Ions_Move_Methods::cal_movement(const int &istep, const ModuleBase::matrix &f, const double &etot)
void Ions_Move_Methods::cal_movement(const int &istep,
                                     const int &force_step,
                                     const ModuleBase::matrix &f,
                                     const double &etot)
{
    ModuleBase::TITLE("Ions_Move_Methods", "init");

    // Ions_Move_Basic::istep = istep;
    Ions_Move_Basic::istep = force_step;

    if (GlobalV::RELAX_METHOD == "bfgs")
    {
        // move_ions
        // output tau
        // check all symmery
        bfgs.start(f, etot);
    }
    else if (GlobalV::RELAX_METHOD == "sd")
    {
        sd.start(f, etot);
    }
    else if (GlobalV::RELAX_METHOD == "cg")
    {
        cg.start(f, etot);
    }
    else if (GlobalV::RELAX_METHOD == "cg_bfgs")
    {
        cg.start(f, etot); // added by pengfei 13-8-10
    }
    else
    {
        ModuleBase::WARNING("Ions_Move_Methods::init", "the parameter GlobalV::RELAX_METHOD is not correct.");
    }
    return;
}
