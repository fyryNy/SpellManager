namespace GOTHIC_NAMESPACE 
{
    zBOOL zCModel::IsAniActive_Union(const zSTRING& aniName)
    {
        auto aniID = this->GetAniIDFromAniName(aniName);
        if(aniID < 0)
        {
            return FALSE;
        }

        auto ani = this->GetAniFromAniID(aniID);
        if(!ani)
        {
            return FALSE;
        }

        auto aniActive = this->IsAniActive(ani);
        if(!aniActive)
        {
            return FALSE;
        }

        return TRUE;
    }
}