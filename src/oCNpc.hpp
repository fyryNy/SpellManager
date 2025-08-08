namespace GOTHIC_NAMESPACE 
{
    void oCNpc::RemoveSpell_Union()
    {
        auto magBook = this->mag_book;

        if(!magBook)
        {
            return;
        }

        auto spell = magBook->GetSelectedSpell();

        
    }
}