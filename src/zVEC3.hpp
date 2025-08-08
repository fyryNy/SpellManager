namespace GOTHIC_NAMESPACE 
{
    inline zVEC3& zVEC3::NormalizeApprox()
    {	
        zREAL d_inv = Alg_SqrtInvApprox (n[VX]*n[VX] + n[VY]*n[VY] + n[VZ]*n[VZ]);
        n[VX] *= d_inv; n[VY] *= d_inv; n[VZ] *= d_inv;
        return *this;
    };

    inline zVEC3 operator ^ (const zVEC3& a, const zVEC3& b)
    {
        return zVEC3(a.n[VY]*b.n[VZ] - a.n[VZ]*b.n[VY], 
                    a.n[VZ]*b.n[VX] - a.n[VX]*b.n[VZ],
                    a.n[VX]*b.n[VY] - a.n[VY]*b.n[VX]);
    }
}