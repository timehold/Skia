/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrCustomStageUnitTest_DEFINED
#define GrCustomStageUnitTest_DEFINED

#include "SkRandom.h"
#include "GrNoncopyable.h"
#include "SkTArray.h"

namespace GrCustomStageUnitTest {
// Used to access the dummy textures in TestCreate procs.
enum {
    kSkiaPMTextureIdx = 0,
    kAlphaTextureIdx = 1,
};
}

#if SK_ALLOW_STATIC_GLOBAL_INITIALIZERS

class GrContext;
class GrEffect;
class GrTexture;

class GrEffectTestFactory : GrNoncopyable {
public:

    typedef GrEffect* (*CreateProc)(SkRandom*, GrContext*, GrTexture* dummyTextures[]);

    GrEffectTestFactory(CreateProc createProc) {
        fCreateProc = createProc;
        GetFactories()->push_back(this);
    }

    static GrEffect* CreateStage(SkRandom* random,
                                      GrContext* context,
                                      GrTexture* dummyTextures[]) {
        uint32_t idx = random->nextRangeU(0, GetFactories()->count() - 1);
        GrEffectTestFactory* factory = (*GetFactories())[idx];
        return factory->fCreateProc(random, context, dummyTextures);
    }

private:
    CreateProc fCreateProc;
    static SkTArray<GrEffectTestFactory*, true>* GetFactories();
};

/** GrEffect subclasses should insert this macro in their declaration to be included in the
 *  program generation unit test.
 */
#define GR_DECLARE_CUSTOM_STAGE_TEST                                           \
    static GrEffectTestFactory gTestFactory;                              \
    static GrEffect* TestCreate(SkRandom*, GrContext*, GrTexture* dummyTextures[2])

/** GrEffect subclasses should insert this macro in their implemenation file. They must then
 *  also implement this static function:
 *      GrEffect* TestCreate(SkRandom*, GrContext*, GrTexture* dummyTextures[2]);
 *  dummyTextures[] are valied textures that they can optionally use for their texture accesses. The
  * first texture has config kSkia8888_PM_GrPixelConfig and the second has kAlpha_8_GrPixelConfig.
  * TestCreate functions are also free to create additional textures using the GrContext.
 */
#define GR_DEFINE_CUSTOM_STAGE_TEST(CustomStage)                               \
    GrEffectTestFactory CustomStage :: gTestFactory(CustomStage :: TestCreate)

#else // !SK_ALLOW_STATIC_GLOBAL_INITIALIZERS

// The unit test relies on static initializers. Just declare the TestCreate function so that
// its definitions will compile.
#define GR_DECLARE_CUSTOM_STAGE_TEST \
    static GrEffect* TestCreate(SkRandom*, GrContext*, GrTexture* dummyTextures[2])
#define GR_DEFINE_CUSTOM_STAGE_TEST(X)

#endif // !SK_ALLOW_STATIC_GLOBAL_INITIALIZERS
#endif
