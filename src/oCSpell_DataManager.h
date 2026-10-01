#include <vector>
#include <memory>
#include "oCSpell_Data.h"

namespace GOTHIC_NAMESPACE 
{
    class oCSpell_DataManager
    {
    public:
        oCSpell_DataManager();
        ~oCSpell_DataManager();
        oCSpell_Data* GetSpellData(int spellId);
        void RestoreParserInstances();

    private:
        std::vector<std::unique_ptr<oCSpell_Data>> sdList;
        static int size_checked;
    };
    inline std::unique_ptr<oCSpell_DataManager> sdManager;
}