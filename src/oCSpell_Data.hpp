namespace GOTHIC_NAMESPACE 
{
    class oCSpell_Data
    {
        friend class oCSpell_DataManager;

    public:
        enum oCSpell_Type : int
        {
            SPELL_TYPE_DEFAULT,
            SPELL_TYPE_PROJECTILE,
            SPELL_TYPE_TRANSFORM,
            SPELL_TYPE_SPREAD,
            SPELL_TYPE_TELEKINESIS,
            SPELL_TYPE_CONTROL
        };

        oCSpell_Data()
		{
            this->pd.spellId = -1;
            this->pd.spellType = oCSpell_Data::oCSpell_Type::SPELL_TYPE_PROJECTILE;
            this->pd.spellEnergyType = NPC_ATR_MANA;

            this->instance = -1;
		}

        void SetInstance(int inst)
        {
            int type, ele;
            this->instance = inst;
            this->name = parser->GetSymbolInfo(inst, type, ele);
        }

        void RestoreParserInstance()
        {
            parser->SetInstance(this->instance, &this->pd);
        }

        int GetDataSize()
        {
            return sizeof(this->pd);
        }

        void* GetDataAdr()
        {
            return &this->pd;
        }

        int instance;
        zSTRING name;

        int GetEnergyType() const
        {
            return this->pd.spellEnergyType;
        }

        int GetId() const
        {
            return this->pd.spellId;
        }

        int GetType() const
        {
            return this->pd.spellType;
        }
    
    private:
        struct ParserData
        {
            int spellId;
            int spellType;
            int spellEnergyType;
        } pd;
    };
}