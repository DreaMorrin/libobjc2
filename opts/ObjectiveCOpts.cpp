#include "llvm/Pass.h"
#include "llvm/Module.h"

#include "ObjectiveCOpts.h"

using namespace llvm;
namespace 
{
  class ObjectiveCOpts : public ModulePass {
    ModulePass *IMPCachePass;
    ModulePass *ClassLookupCachePass;
    ModulePass *ClassMethodInliner;
    FunctionPass *GNUNonfragileIvarPass;
    FunctionPass *GNULoopIMPCachePass;

    public:
    static char ID;
    ObjectiveCOpts() : ModulePass(ID) {
      IMPCachePass = createClassIMPCachePass();
      ClassLookupCachePass = createClassLookupCachePass();
      ClassMethodInliner = createClassMethodInliner();
      GNUNonfragileIvarPass = createGNUNonfragileIvarPass();
      GNULoopIMPCachePass = createGNULoopIMPCachePass();
    }
    virtual ~ObjectiveCOpts() {
      delete IMPCachePass;
      delete ClassMethodInliner;
      delete ClassLookupCachePass;
      delete GNULoopIMPCachePass;
      delete GNUNonfragileIvarPass;
    }

    virtual bool runOnModule(Module &Mod) {
      bool modified;
      modified = IMPCachePass->runOnModule(Mod);
      modified |= ClassLookupCachePass->runOnModule(Mod);
      modified |= ClassMethodInliner->runOnModule(Mod);

      for (Module::iterator F=Mod.begin(), fend=Mod.end() ;
          F != fend ; ++F) {

        if (F->isDeclaration()) { continue; }
        modified |= GNUNonfragileIvarPass->runOnFunction(*F);
        modified |= GNULoopIMPCachePass->runOnFunction(*F);
      }

      return modified;
    };
  };

  char ObjectiveCOpts::ID = 0;
  RegisterPass<ObjectiveCOpts> X("gnu-objc", 
          "Run all of the GNUstep Objective-C runtimm optimisations");
}