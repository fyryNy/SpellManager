namespace GOTHIC_NAMESPACE 
{
    bool zCModel::IsAniActive_Union(const zSTRING& aniName)
    {
        auto aniID = this->GetAniIDFromAniName(aniName);
        if(aniID < 0)
        {
            return false;
        }

        auto ani = this->GetAniFromAniID(aniID);
        if(!ani)
        {
            return false;
        }

        auto aniActive = this->IsAniActive(ani);
        if(!aniActive)
        {
            return false;
        }

        return true;
    }
}