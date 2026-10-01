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

        oCSpell_Data();
        void SetInstance(int inst);
        void RestoreParserInstance();
        int GetDataSize();
        void* GetDataAdr();
        int instance;
        zSTRING name;
        int GetEnergyType() const;
        int GetId() const;
        int GetType() const;
        int GetIsInvestSpell() const;
    
    private:
        struct ParserData
        {
            int spellId;
            int spellType;
            int spellEnergyType;
            int spellIsInvestSpell;
        } pd;
    };
}