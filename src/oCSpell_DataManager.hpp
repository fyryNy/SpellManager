namespace GOTHIC_NAMESPACE 
{
    class oCSpell_DataManager
    {
    public:
        oCSpell_DataManager()
        {
            if(!parser)
            {
                return;
            }

            auto className = zSTRING{ "C_SPELL_DATA" };
            auto classIndex = parser->GetIndex(className);
            if(classIndex < 0)
            {
                return;
            }

            int pos = 0;
            while (pos >= 0)
            {
                pos = parser->GetInstance(classIndex, pos + 1);
                if(pos >= 0)
                {
                    auto spellData = std::make_unique<oCSpell_Data>();

                    if(!this->size_checked)
                    {
                        parser->CheckClassSize(className, spellData->GetDataSize());
                        this->size_checked = TRUE;
                    }

                    parser->CreateInstance(pos, spellData->GetDataAdr());
                    spellData->SetInstance(pos);
                    this->sdList.push_back(std::move(spellData));
                }
            }
        }

        ~oCSpell_DataManager()
        {

        }
  
        oCSpell_Data* GetSpellData(int spellId)
        {
            if(spellId < 0)
            {
                return nullptr;
            }

            for (auto& spellData : this->sdList)
            {
                if(spellData->pd.spellId == spellId)
                {
                    return spellData.get();
                }
            }

            return nullptr;
        }

        void RestoreParserInstances()
        {
            for (auto& spellData : this->sdList)
            {
                spellData->RestoreParserInstance();
            }
        }

    private:
        std::vector<std::unique_ptr<oCSpell_Data>> sdList;
        static int size_checked;
    };
  
    std::unique_ptr<oCSpell_DataManager> sdManager;

    int oCSpell_DataManager::size_checked = FALSE;
}