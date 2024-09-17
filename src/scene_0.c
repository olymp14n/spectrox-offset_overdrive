#include "scene_0.h"
#include <GL/glu.h>
#include <math.h>
#include "config/config.h"
#include "fw/types.h"
#include "fw/render.h"
#include "fw/lut.h"
#include "fw/easing.h"
#include "fw/animation.h"
#include "fw/font.h"
#include "fw/projection.h"
#include "resource.h"
#include "fx/dot_tunnel.h"
#include "fx/dot_sphere.h"
#include "fx/cylinder_twist.h"
#include "fw/math.h"
#include "fw/quad.h"
#include "fw/stencil.h"
#include "fx/endless_grid.h"
#include "fx/greetings_pseudo_3d.h"
#include "fx/hextiles.h"
#include "fx/overlay.h"
#include "fx/isologo.h"
#include "fx/love.h"
#include "fx/text_3d.h"
#include "fx/anim_text.h"
#include "fw/sync.h"
#include "fw/broadcast.h"

static fw_sync_data _syncData;

static fx_anim_text_meta _presentsTextMeta1;

static fx_dot_tunnel _dotTunnel;
static fx_dot_tunnel_dot_color *_dotTunnelDefaultColors;
static fx_dot_tunnel_dot_color *_dotTunnelRainbowColors;

#define NUM_DOT_TUNNEL_TEXT_PARAMS 4
static fx_text_3d_params _dotTunnelTextParams[NUM_DOT_TUNNEL_TEXT_PARAMS];

typedef struct {
    fw_vec4f pos;
    fw_vec4f sineAmp;
    fw_vec4f sineOffset;
    fw_vec4f sineSpeed;
    fw_vec4f palIndex;
} text_3d_anim_params;

static text_3d_anim_params _dotTunnelText3dAnimParams[NUM_DOT_TUNNEL_TEXT_PARAMS];

static const fw_vec4f _dotTunnelText3dPosInits[NUM_DOT_TUNNEL_TEXT_PARAMS] = {
    {-4,2,-100},
    {-5.5,0,-100},
    {7.0,-1,-100},
    {-1.0,1.0,-100},
};

static fw_vec4i _dotTunnelText3dPalette[] = {
    {70, 70, 70,255},
    {100, 100, 100,255},
    {140, 140, 140,255},
    {140, 155, 190,255},
    {138, 179, 199,255},
    {149, 213, 212,255},
    {188, 239, 205,255},
    {242, 240, 179,255},
    {237, 237, 237,255},
};
static const unsigned char _dotTunnelText3dPaletteSize = LEN(_dotTunnelText3dPalette);

// Size: 23
static fw_vec4i _presentsPalette[] = {
    {76,44,49,255},
    {106,54,51,255},
    {137,64,54,255},
    {137,64,54,255},
    {144,95,37,255},
    {137,64,54,255},
    {144,95,37,255},
    {144,95,37,255},
    {187,119,109,255},
    {144,95,37,255},
    {187,119,109,255},
    {187,119,109,255},
    {171,171,171,255},
    {187,119,109,255},
    {171,171,171,255},
    {171,171,171,255},
    {208,220,113,255},
    {171,171,171,255},
    {208,220,113,255},
    {208,220,113,255},
    {255,255,255,255},
    {208,220,113,255},
    {255,255,255,255},
};
static const unsigned char _presentsPaletteSize = LEN(_presentsPalette);

static fx_dot_sphere _bounceSphere;
static fx_dot_sphere _twirlSphere;

static fw_vec4f *_isoLogoTransitionLinePositions;
static const int _numIsoLogoTransitionLinePositions = FW_RENDER_WIDTH/2;

static fx_isologo_render_meta _isoLogoMeta;

static const fw_vec4f _cylinderCheckerRowFlashColor = {32*2,110*2,255,255};
static const fw_vec4f _cylinderCheckerRowFinalColor = {13*2,45*2,75*2,255};
static const fw_vec4f _cylinderCheckerRowFinalColorDimmed = {13,45,75,255};
static const fw_vec4f _cylinderCheckerRowFinalColorDimmed2 = {13,45,75,150};

static fx_cylinder_params _cylinderParams;


typedef struct {
    int rowNum;
    int note;
} sync_row_note;


static fw_vec4i _endlessGridSliceDims[] = {
    {0, 0, 19, 270},
    {19, 0, 19, 270},
    {38, 0, 19, 270},
    {57, 0, 46, 270},
    {103, 0, 46, 270},
    {149, 0, 46, 270},
    {195, 0, 46, 270},
    {241, 0, 46, 270},
    {287, 0, 19, 270},
    {306, 0, 19, 270},
    {325, 0, 21, 270},
};

static sync_row_note _endlessGridRowNoteMap[] = {
    {656, 5},       // 0
    {656+4, 4},     // 1
    {656+4, 6},     // 2
    {656+8, 3},     // 3
    {656+8, 7},     // 4
    {64+656, 2},    // 5
    {64+656, 8},    // 6
    {64+656+4, 1},  // 7
    {64+656+4, 9},  // 8
    {64+656+8, 0},  // 9
    {64+656+8, 10}, // 10
};
static int _endlessGridSliceAlphas[LEN(_endlessGridRowNoteMap)];
static BOOL _endlessGridStencilStates[LEN(_endlessGridRowNoteMap)];
const static int _endlessGridRowNoteMapLength = LEN(_endlessGridRowNoteMap);



static sync_row_note *_endScrollerRowNoteMap;
static int _endScrollerRowNoteMapLength;
static fw_vec4i _endScrollerRowColors[] = {
    {106,54,51,0},
    {137,64,54,0},
    {106,54,51,0},
    {144,95,37,0},
    {187,119,109,0},
    {144,95,37,0},
    {187,119,109,0},
    {171,171,171,0},
    {208,220,113,0},
    {171,171,171,0},
    {208,220,113,0},
    {208,220,113,0},
    {255,255,255,0},
};
static const int _numEndScrollerRowColors = LEN(_endScrollerRowColors);


enum anim_names_enum {
    ANIM_PRESENTS,
    ANIM_PRESENTS_TEXT1,
    ANIM_TITLE_LOGO,
    ANIM_ENDLESS_GRID,
    ANIM_TWIRL_SPHERE,
    ANIM_MOUNTAINS_SCROLLER,
    ANIM_BOUNCE_SPHERES,
    ANIM_BOUNCE_SPHERES_LOGO_SYNC,
    ANIM_LOVE,
    ANIM_GREETS,
    ANIM_ISO_PATTERN,
    ANIM_ISO_PATTERN_IN,
    ANIM_ISO_PATTERN_OUT,
    ANIM_ISO_LOGO_TRANSITION,
    ANIM_ISO_LOGO_TRANSITION_LINE,
    ANIM_DOT_TUNNEL,
    ANIM_CYLINDER_TWIST_TRANSITION,
    ANIM_CYLINDER_TWIST_TEXT1,
    ANIM_CYLINDER_TWIST_TEXT2,
    ANIM_CYLINDER_TWIST,
    ANIM_END_BLINDS_TRANSITION,
    ANIM_END_SCROLLER,
};

///////////////////////////////////////////////////////////////////////////////
// STRINGS
///////////////////////////////////////////////////////////////////////////////

static char *_presentsText1[] = {
    "IN 2024",
    "S*P*E*C*T*R*O*X",
    "IS BACK WITH...",
};
static int _numLinesPresentsText1 = LEN(_presentsText1);


static char *_endScrollerText[] = {
    " YOU HAVE BEEN WATCHING",
    "",
    "",
    "        O F F S E T",
    "     O V E R D R I V E",
    "",
    "",
    "    2024 BY SPECTROX",
    "",
    "",
    "CODE",
    "GRAPHICS         OLYMPIAN",
    "MUSIC"
    "",
    "",
    "THE MAIN TUNE IS A COVER ",
    "VERSION OF ALEX WINSTON'S ",
    "AY-CHIPTUNE \"ADAPTATIONS\". ",
    "THANKS, ALEX, FOR LETTING",
    "ME BUTCHER IT!"
    "",
    "",
    "",
    "            % % %",
    "",
    "",
    "THIS WAS INITIALLY PLANNED",
    "AS A ONE-SCREEN INTRO AND",
    "WAS SUPPOSED TO BE",
    "RELEASED EARLIER THIS YEAR",
    "OUTSIDE OF A PARTY.",
    "",
    "HOWEVER, AS I KEPT ADDING ",
    "MORE SCENES, I DECIDED TO AIM ",
    "FOR A RELEASE AT DEADLINE ",
    "2024. SO, A BIG HELLO TO ",
    "ALL THE LOVELY PEOPLE AT",
    "DEADLINE!",
    "",
    "SOME PERSONAL GREETS AND ",
    "MESSAGES COMING UP.",
    "",
    "$ DEPECHE / SPREADPOINT",
    "  IT'S BEEN NICE CHATTING WITH",
    "  YOU! LOOKING FORWARD TO ",
    "  MORE SPREADPOINT STUFF!",
    "",
    "$ FERRIS / LOGICOMA",
    "  WITHOUT \"SQUISHY\" THIS WOULD",
    "  BE A DEMO ;)",
    "",
    "$ LE FRED",
    "  EXCITED FOR MORE AWESOME",
    "  TRACKS FROM YOU!",
    "",
    "$ MOTION / ARTSTATE",
    "  KEEP THE RELEASES COMING!",
    "",
    "$ MYSTRA / NECTARINE",
    "  THANKS FOR PUTTING MY SONGS",
    "  UP ON NECTARINE RADIO!",
    "",
    "$ OZAN / TEK",
    "  THIS TIME I DIDN'T FORGET THE",
    "  TEK GREETS XD",
    "",
    "$ RUDI / SSR",
    "  KEEP UP THE YT UPLOADS!",
    "",
    "$ VIRGILL / ALCATRAZ",
    "  THANKS FOR THE SHOUT-OUT ",
    "  IN \"SOIL\"! IT'S SUCH A GREAT ",
    "  AMIGA RELEASE!",
    "",
    "$ ZAK / FROZAK",
    "  THANKS FOR ALL THE ADVICE!",
    "",
    "",
    "            % % %",
    "",
    "",
    "IT'S TIME TO WRAP THINGS UP.",
    " SEE YOU IN THE NEXT ONE!",
    "",
    "",
    "       T H E   E N D"
    "",
    "",
    "              %  ",

};
static int _numLinesEndScrollerText = LEN(_endScrollerText);

///////////////////////////////////////////////////////////////////////////////
// INITIAL KEYFRAME VARIABLES
///////////////////////////////////////////////////////////////////////////////
static const fw_vec4f _globalClearColorInit = {46/255.f,34/255.f,47/255.f,1};

static const fw_vec4f _presentsBGPosInit = {0,-200};

static const fw_vec4f _titleBgPosInit = {0,-30};
static const fw_vec4f _titleBgAlphaInit = {1.f};
static const fw_vec4f _titleOutlineStencilPosInit = {0,0};
static const fw_vec4f _titleStencilPosInit = {0,295};
static const fw_vec4f _titleRefelectionPosInit = {-20};
static const fw_vec4f _titlePosInit = {3,110};
static const fw_vec4f _title2PosInit = {32,96};
static const fw_vec4f _title3PosInit = {32,170};
static const fw_vec4f _endlessGridColorAlphaInit = {0};
static const fw_vec4f _endlessGridMonoAlphaInit = {255};

static const fw_vec4f _twirlSphereScaleInit = {0};
static const fw_vec4f _twirlSpherePosInit = {0};
static const fw_vec4f _twirlSphereShadowPosInit = {-1};
static const fw_vec4f _twirlSphereBgTopInit = {0};
static const fw_vec4f _twirlSphereBgBottomInit = {0};

static const fw_vec4f _twirlSphereTrellisTopPosInit = {FW_RENDER_WIDTH};
static const fw_vec4f _twirlSphereTrellisBottomPosInit = {-FW_RENDER_WIDTH};

static const fw_vec4f _mountainsTextPosInit = {15.6,-.7,-7};
static const fw_vec4f _mountainsPosInit = {0,171};
static const fw_vec4f _mountainsShadowScaleInit = {0};
static const fw_vec4f _mountainsSunPosInit = {0,-100};
static const fw_vec4f _mountainsLogoPosInit = {0,50};

static const fw_vec4f _bounceSpheresBayerQuadPosInit = {-40};
static const fw_vec4f _bounceSpheresBayerQuadPos2Init = {-40};

static const fw_vec4f _loveTransitionQuadPosInit = {0,0,-1};
static const fw_vec4f _loveTransitionQuadRotInit = {1,0,0,180};

static const fw_vec4f _greetsStencil1PosInit = {270};
static const fw_vec4f _greetsStencil2PosInit = {270};

static const fw_vec4f _isoLogoShadowPosInit = {18,FW_RENDER_HEIGHT};

static const fw_vec4f _dotTunnelAmpInit = {0,0};
static const fw_vec4f _dotTunnelOverlayScaleInit = {0};
static const fw_vec4f _dotTunnelBorderTopPosInit = {FW_RENDER_HEIGHT/2};
static const fw_vec4f _dotTunnelBorderBottomPosInit = {FW_RENDER_HEIGHT/2};

static const fw_vec4f _cylinderCheckerZoomInit = {1.03};
static const fw_vec4f _cylinderCheckerMiddelSpreadInit = {2};
static const fw_vec4f _cylinderYDeltasInit = {-6,-6,-6};
static const fw_vec4f _cylinderTwistYSineAmpInit = {2,2,2.5f};
static const fw_vec4f _cylinderTwistSpeedInit = {0};
static const fw_vec4f _cylinderTwistVarianceInit = {.005f};
static const fw_vec4f _cylinderTwistDistanceInit = {.6f, .3f};
static const fw_vec4f _cylinderTwistBaseRadiusInit = {.3f,.3f,.3f};
static const fw_vec4f _cylinderTwistTopRadiusInit = {.3f,.3f,.3f};
static const fw_vec4f _cylinderTwistSlicesInit = {4,4,4};

///////////////////////////////////////////////////////////////////////////////
// KEY FRAME VARIABLES
///////////////////////////////////////////////////////////////////////////////
static fw_vec4f _dummyKeyVal = {0};

static fw_vec4f _globalClearColor = _globalClearColorInit;

static fw_vec4f _presentsBGPos = _presentsBGPosInit;

static fw_vec4f _titleBgPos = _titleBgPosInit;
static fw_vec4f _titleBgAlpha = _titleBgAlphaInit;
static fw_vec4f _titleOutlineStencilPos = _titleOutlineStencilPosInit;
static fw_vec4f _titleStencilPos = _titleStencilPosInit;
static fw_vec4f _titleRefelectionPos = _titleRefelectionPosInit;
static fw_vec4f _titlePos = _titlePosInit;
static fw_vec4f _title2Pos = _title2PosInit;
static fw_vec4f _title3Pos = _title3PosInit;
static fw_vec4f _endlessGridColorAlpha = _endlessGridColorAlphaInit;
static fw_vec4f _endlessGridMonoAlpha = _endlessGridMonoAlphaInit;

static fw_vec4f _endlessGridBorderDims[2];
static fw_vec4f _endlessGridBorderStencilDim;

static fw_vec4f _twirlSphereScale = _twirlSphereScaleInit;
static fw_vec4f _twirlSpherePos = _twirlSpherePosInit;
static fw_vec4f _twirlSphereShadowPos = _twirlSphereShadowPosInit;

static fw_vec4f _twirlSphereBgTop = _twirlSphereBgTopInit;
static fw_vec4f _twirlSphereBgBottom = _twirlSphereBgBottomInit;

static fw_vec4f _twirlSphereTrellisTopPos = _twirlSphereTrellisTopPosInit;
static fw_vec4f _twirlSphereTrellisBottomPos = _twirlSphereTrellisBottomPosInit;

static fw_vec4f _mountainsSunsetBgDims[6];
static fw_vec4f _mountainsLogoColor;
static fw_vec4f _mountainsTextPos = _mountainsTextPosInit;
static fw_vec4f _mountainsPos = _mountainsPosInit;
static fw_vec4f _mountainsShadowScale = _mountainsShadowScaleInit;
static fw_vec4f _mountainsSunPos = _mountainsSunPosInit;
static fw_vec4f _mountainsLogoPos = _mountainsLogoPosInit;

static fw_vec4f _bounceSpheresBayerQuadPos = _bounceSpheresBayerQuadPosInit;
static fw_vec4f _bounceSpheresBayerQuad2Pos = _bounceSpheresBayerQuadPos2Init;

static fw_vec4f _loveTransitionQuadPos = _loveTransitionQuadPosInit;
static fw_vec4f _loveTransitionQuadRot = _loveTransitionQuadRotInit;
static fw_vec4f _loveTransitionStencilEnabled = {0};
static fw_vec4f _loveTransitionEnabled = {1};
static fw_vec4f _loveRainbowPos = {0};
static fw_vec4f _loveTextPos = {0};

static fw_vec4f _hexTilesProgress = {0};

static fw_vec4f _greetsStencil1Pos = _greetsStencil1PosInit;
static fw_vec4f _greetsStencil2Pos = _greetsStencil2PosInit;

static fw_vec4f _greetsTransitQuadDim = {FW_RENDER_WIDTH};

static fw_vec4f _isoPatternBGColor = {0};
static fw_vec4f _isoPatternGradientAlpha = {0};

static fw_vec4f _isoLogoShadowPos = _isoLogoShadowPosInit;
static fw_vec4f _isoLogoAmp = {0};
static fw_vec4f _isoLogoPos = {0};
static fw_vec4f _isoLogoPos2 = {0};
static fw_vec4f _isoLogoVerticalSpread = {166};
static fw_vec4f _isoLogoPercentage = {0};
static fw_vec4f _isoLogoOutlineAlpha = {0};
static fw_vec4f _isoLogoSrcDst = {FX_ISO_LOGO_MIN_Y, FX_ISO_LOGO_MAX_Y};

static fw_vec4f _dotTunnelAmp = _dotTunnelAmpInit;
static fw_vec4f _dotTunnelOverlayScale = _dotTunnelOverlayScaleInit;
static fw_vec4f _dotTunnelBorderTopPos = _dotTunnelBorderTopPosInit;
static fw_vec4f _dotTunnelBorderBottomPos = _dotTunnelBorderBottomPosInit;
static fw_vec4f _dotTunnelIsBorderTopFG = {0};



static fw_vec4f _cylinderOverlayColor = {0};
static fw_vec4f _cylinderCheckerRowPositions[] = {
    {.32, -.132},
    {-.57, -.132},
    {.32, -.132},
    {.32, -.132},
};
static fw_vec4f _cylinderCheckerRowColors[] = {
    {13*2,45*2,75*2,255},
    {13*2,45*2,75*2,255},
    {13*2,45*2,75*2,255},
    {13*2,45*2,75*2,255},
};
static fw_vec4f _cylinderCheckerRowRotations[]  = {
    {0},
    {1,0,0,180},
    {0,0,1,90},
    {0},
};

#define NUM_CYLINDER_TEXT_LINES  13
static fw_vec4f _cylinderText1PaletteIndexes[NUM_CYLINDER_TEXT_LINES];
static fw_vec4f _cylinderText2PaletteIndexes[NUM_CYLINDER_TEXT_LINES];


static fw_vec4f _cylinderCheckerGroundLayerColor = _cylinderCheckerRowFinalColor;
static fw_vec4f _cylinderCheckerZoom = _cylinderCheckerZoomInit;
static fw_vec4f _cylinderCheckerMiddelSpread = _cylinderCheckerMiddelSpreadInit;
static fw_vec4f _cylinderYDeltas = _cylinderYDeltasInit;
static fw_vec4f _cylinderTwistYSineAmp = _cylinderTwistYSineAmpInit;
static fw_vec4f _cylinderCheckerSpeed = _cylinderTwistSpeedInit;
static fw_vec4f _cylinderTwistVariance = _cylinderTwistVarianceInit;
static fw_vec4f _cylinderTwistDistance = _cylinderTwistDistanceInit;
static fw_vec4f _cylinderTwistBaseRadius = _cylinderTwistBaseRadiusInit;
static fw_vec4f _cylinderTwistTopRadius = _cylinderTwistTopRadiusInit;
static fw_vec4f _cylinderTwistSlices = _cylinderTwistSlicesInit;
static fw_vec4f _cylinderTwistTransitionOutQuadHeight;

static fw_vec4f _creditsBGHeight;
static fw_vec4f _endScrollerLineDims[2];

///////////////////////////////////////////////////////////////////////////////
// ANIM START / STOP DECLARATIONS
///////////////////////////////////////////////////////////////////////////////
static void animStartedPresents(fw_animation *anim, fw_timer_data *time);
static void animCompletedPresents(fw_animation *anim, fw_timer_data *time);

static void animStartedTitleLogo(fw_animation *anim, fw_timer_data *time);
static void animCompletedTitleLogo(fw_animation *anim, fw_timer_data *time);

static void animStartedEndlessGrid(fw_animation *anim, fw_timer_data *time);
static void animCompletedEndlessGrid(fw_animation *anim, fw_timer_data *time);

static void animStartedTwirlSphere(fw_animation *anim, fw_timer_data *time);
static void animCompletedTwirlSphere(fw_animation *anim, fw_timer_data *time);

static void animStartedMountainsScroller(fw_animation *anim, fw_timer_data *time);
static void animCompletedMountainsScroller(fw_animation *anim, fw_timer_data *time);

static void animStartedBounceSpheres(fw_animation *anim, fw_timer_data *time);
static void animCompletedBounceSpheres(fw_animation *anim, fw_timer_data *time);

static void animStartedLove(fw_animation *anim, fw_timer_data *time);
static void animCompletedLove(fw_animation *anim, fw_timer_data *time);

static void animStartedGreets(fw_animation *anim, fw_timer_data *time);
static void animCompletedGreets(fw_animation *anim, fw_timer_data *time);

static void animStartedIsoPattern(fw_animation *anim, fw_timer_data *time);
static void animCompletedIsoPattern(fw_animation *anim, fw_timer_data *time);

static void animStartedIsoLogoTransition(fw_animation *anim, fw_timer_data *time);
static void animCompletedIsoLogoTransition(fw_animation *anim, fw_timer_data *time);

static void animStartedDotTunnel(fw_animation *anim, fw_timer_data *time);
static void animCompletedDotTunnel(fw_animation *anim, fw_timer_data *time);

static void animStartedCylinderTwistTransition(fw_animation *anim, fw_timer_data *time);
static void animCompletedCylinderTwistTransition(fw_animation *anim, fw_timer_data *time);

static void animStartedCylinderTwist(fw_animation *anim, fw_timer_data *time);
static void animCompletedCylinderTwist(fw_animation *anim, fw_timer_data *time);

static void animStartedEndBlindsTransition(fw_animation *anim, fw_timer_data *time);
static void animCompletedEndBlindsTransition(fw_animation *anim, fw_timer_data *time);

static void animStartedEndScroller(fw_animation *anim, fw_timer_data *time);
static void animCompletedEndScroller(fw_animation *anim, fw_timer_data *time);


///////////////////////////////////////////////////////////////////////////////
// KEY FRAMES
///////////////////////////////////////////////////////////////////////////////

static fw_keyframe _keysPresents[] = {
    {0, 6, _presentsBGPosInit, {0, -650}, LinearInterpolation, &_presentsBGPos},

    // Wait until text anim is over.
    {15, 23, {0}, {0}, LinearInterpolation, &_dummyKeyVal},
};

static fw_keyframe _keysTitleLogo[] = {
    {1, 5, _titleBgPosInit, {0, 300.f}, LinearInterpolation, &_titleBgPos},
    {5, 9, {0, 300.f}, _titleBgPosInit, LinearInterpolation, &_titleBgPos},

    {2.6, 4.1, _titleOutlineStencilPosInit, {0,90}, LinearInterpolation, &_titleOutlineStencilPos},
    {5, 9, _titleStencilPosInit, {0,0}, LinearInterpolation, &_titleStencilPos}, // Timing corresponds to _titleBgPos above.

    {9, 10, _titleRefelectionPosInit, {410}, LinearInterpolation, &_titleRefelectionPos},

    // Show title logo a bit longer.
    {11, 11, {0}, {0}, LinearInterpolation, &_dummyKeyVal},
};

static fw_keyframe _keysEndlessGrid[] = {
    {6.2, 7.2, _titleBgAlphaInit, {0}, LinearInterpolation, &_titleBgAlpha},

    {18, 20, {0,-2, FW_RENDER_WIDTH,2}, {0,20, FW_RENDER_WIDTH,2}, QuadraticEaseInOut, &_endlessGridBorderDims[0]}, // Top
    {18, 20, {0,FW_RENDER_HEIGHT+2, FW_RENDER_WIDTH,2}, {0,FW_RENDER_HEIGHT-20-2, FW_RENDER_WIDTH,2}, QuadraticEaseInOut, &_endlessGridBorderDims[1]}, // Bottom
    {18, 20, {0,-2, FW_RENDER_WIDTH,FW_RENDER_HEIGHT+2}, {0,20, FW_RENDER_WIDTH,230}, QuadraticEaseInOut, &_endlessGridBorderStencilDim},

    {13.5, 13.5, {255}, {255}, LinearInterpolation, &_endlessGridColorAlpha},
    {13.5, 17.5, _endlessGridMonoAlphaInit, {0}, LinearInterpolation, &_endlessGridMonoAlpha},

    {21, 21.75, {0,20, FW_RENDER_WIDTH,2}, {0,135, FW_RENDER_WIDTH,2}, QuadraticEaseInOut, &_endlessGridBorderDims[0]}, // Top
    {21, 21.75, {0,FW_RENDER_HEIGHT-20-2, FW_RENDER_WIDTH,2}, {0,135, FW_RENDER_WIDTH,2}, QuadraticEaseInOut, &_endlessGridBorderDims[1]}, // Bottom

    {21, 21.75,   {0,20, FW_RENDER_WIDTH,230}, {0,135, FW_RENDER_WIDTH,0}, QuadraticEaseInOut, &_endlessGridBorderStencilDim},
};

static fw_keyframe _keysTwirlSphere[] = {
    // Swipe to top.
    {0, .75, {0,135, 346,2}, {0,0, 346,2}, QuadraticEaseInOut, &_endlessGridBorderDims[0]}, // Top
    {0, .75, {0,135, 346,2}, {0,0, 346,2}, QuadraticEaseInOut, &_endlessGridBorderDims[1]}, // Bottom

    // Swipe to bottom.
    {.75, 1.5, {0,0, 346,2}, {0,268, 346,2}, QuadraticEaseInOut, &_endlessGridBorderDims[0]}, // Top
    {.75, 1.5, {0,0, 346,2}, {0,268, 346,2}, QuadraticEaseInOut, &_endlessGridBorderDims[1]}, // Bottom

    // Top BG swipe down.
    {.75, 1.5, {0}, {268}, QuadraticEaseInOut, &_twirlSphereBgTop},

    // Swipe to middle.
    {1.5, 2.25, {0,268, 346,2}, {0,134, 346,2}, QuadraticEaseInOut, &_endlessGridBorderDims[0]}, // Top
    {1.5, 2.25, {0,268, 346,2}, {0,134, 346,2}, QuadraticEaseInOut, &_endlessGridBorderDims[1]}, // Bottom

    // Bottom BG swipe up.
    {1.5, 2.25, {0}, {-136}, QuadraticEaseInOut, &_twirlSphereBgBottom},

    // Reduce to dot.
    {2.25, 3, {0,134, 346,2}, {172,134, 2,2}, QuadraticEaseInOut, &_endlessGridBorderDims[0]}, // Top
    {2.25, 3, {0,134, 346,2}, {172,134, 2,2}, QuadraticEaseInOut, &_endlessGridBorderDims[1]}, // Bottom

    {3.2, 5, _twirlSphereScaleInit, {2,2,2}, QuadraticEaseInOut, &_twirlSphereScale},
    {5, 6, {2,2,2}, {1.5,1.5,1.5}, QuadraticEaseInOut, &_twirlSphereScale},

    {5, 6, _twirlSphereShadowPosInit, {.8}, QuadraticEaseOut, &_twirlSphereShadowPos},

    {15, 17, _twirlSpherePosInit, {-4.8}, BackEaseIn, &_twirlSpherePos},
    {15, 16, {.8}, _twirlSphereShadowPosInit, QuadraticEaseIn, &_twirlSphereShadowPos},

    {7, 8, _twirlSphereTrellisBottomPosInit, {0}, QuadraticEaseOut, &_twirlSphereTrellisBottomPos},
    {8, 9, _twirlSphereTrellisTopPosInit, {0}, QuadraticEaseOut, &_twirlSphereTrellisTopPos},

    {17, 18, {0}, _twirlSphereTrellisTopPosInit, QuadraticEaseOut, &_twirlSphereTrellisTopPos},
    {17, 18, {0}, _twirlSphereTrellisBottomPosInit, QuadraticEaseOut, &_twirlSphereTrellisBottomPos},
};

static fw_keyframe _keysMountainsScroller[] = {
    {1, 2, _mountainsPosInit, {0,96}, QuadraticEaseOut, &_mountainsPos},
    {24, 25, {0,96}, _mountainsPosInit, QuadraticEaseIn, &_mountainsPos},

    {2, 3, _mountainsShadowScaleInit, {0,-.2f}, QuadraticEaseOut, &_mountainsShadowScale},
    {23, 24, {0,-.2f}, _mountainsShadowScaleInit, BackEaseIn, &_mountainsShadowScale},

    {2, 3, {0,135}, {0,100}, QuadraticEaseOut, &_mountainsSunPos},
    {23, 24, {0,100}, {0,135}, BackEaseIn, &_mountainsSunPos},
    {24, 24, _mountainsSunPosInit, _mountainsSunPosInit, QuadraticEaseIn, &_mountainsSunPos},

    {4.f+0*.06f, 4.f+1*.06f, {0,0,0,0}, {64,49,141,255}, LinearInterpolation, &_mountainsLogoColor},
    {4.f+1*.06f, 4.f+2*.06f, {64,49,141,255}, {139,63,150,255}, LinearInterpolation, &_mountainsLogoColor},
    {4.f+2*.06f, 4.f+3*.06f, {139,63,150,255}, {85,160,73,255}, LinearInterpolation, &_mountainsLogoColor},
    {4.f+3*.06f, 4.f+4*.06f, {85,160,73,255}, {184,105,98,255}, LinearInterpolation, &_mountainsLogoColor},
    {4.f+4*.06f, 4.f+5*.06f, {184,105,98,255}, {103,182,189,255}, LinearInterpolation, &_mountainsLogoColor},
    {4.f+5*.06f, 4.f+6*.06f, {103,182,189,255}, {191,206,114,255}, LinearInterpolation, &_mountainsLogoColor},
    {4.f+6*.06f, 4.f+7*.06f, {191,206,114,255}, {255,255,255,255}, LinearInterpolation, &_mountainsLogoColor},

    {5.5, 23, _mountainsTextPosInit, {-15.6,-.7,-7}, LinearInterpolation, &_mountainsTextPos},
};

static const float _bounceSpheresDelay = 6;
static fw_keyframe _keysBounceSpheres[] = {
    {0, 0, {255,255,255,255}, {255,255,255,255}, LinearInterpolation, &_mountainsLogoColor},

    {0, .8, _mountainsLogoPosInit, {0,100}, QuadraticEaseInOut, &_mountainsLogoPos},
    {.8, 1.6, {0,100}, {0,80}, QuadraticEaseInOut, &_mountainsLogoPos},
    {1.6, 2.2, {0,80}, {0,90}, QuadraticEaseInOut, &_mountainsLogoPos},

    {0, 0, {0}, {0,238, FW_RENDER_WIDTH,32}, LinearInterpolation, &_mountainsSunsetBgDims[5]},
    {0, 0, {0}, {0,170, FW_RENDER_WIDTH,68}, LinearInterpolation, &_mountainsSunsetBgDims[4]},
    {0, 0, {0}, {0,128, FW_RENDER_WIDTH,42}, LinearInterpolation, &_mountainsSunsetBgDims[3]},
    {0, 0, {0}, {0,86, FW_RENDER_WIDTH,42}, LinearInterpolation, &_mountainsSunsetBgDims[2]}, // Contains logo.
    {0, 0, {0}, {0,42, FW_RENDER_WIDTH,44}, LinearInterpolation, &_mountainsSunsetBgDims[1]},
    {0, 0, {0}, {0,0, FW_RENDER_WIDTH,42}, LinearInterpolation, &_mountainsSunsetBgDims[0]},

    {1.0, 2.0, {0,238, FW_RENDER_WIDTH,32}, {0,238+32/2, FW_RENDER_WIDTH,0}, BackEaseOut, &_mountainsSunsetBgDims[5]},
    {1.5, 2.5, {0,0, FW_RENDER_WIDTH,42}, {0,0+42/2, FW_RENDER_WIDTH,0}, BackEaseOut, &_mountainsSunsetBgDims[0]},
    {2.0, 3.0, {0,170, FW_RENDER_WIDTH,68}, {0,170+68/2, FW_RENDER_WIDTH,0}, BackEaseOut, &_mountainsSunsetBgDims[4]},
    {2.5, 3.5, {0,42, FW_RENDER_WIDTH,44}, {0,42+44/2, FW_RENDER_WIDTH,0}, BackEaseOut, &_mountainsSunsetBgDims[1]},
    {3.0, 4.0, {0,128, FW_RENDER_WIDTH,42}, {0,128+42/2, FW_RENDER_WIDTH,0}, BackEaseOut, &_mountainsSunsetBgDims[3]},
    {3.5, 4.5, {0,86, FW_RENDER_WIDTH,42}, {0,86+42/2, FW_RENDER_WIDTH,0}, BackEaseOut, &_mountainsSunsetBgDims[2]}, // Contains logo.

    {21.5, 22.4, _bounceSpheresBayerQuadPosInit, {FW_RENDER_WIDTH}, QuadraticEaseInOut, &_bounceSpheresBayerQuadPos},
    {21.541, 22.441, _bounceSpheresBayerQuadPos2Init, {FW_RENDER_WIDTH}, QuadraticEaseInOut, &_bounceSpheresBayerQuad2Pos},

    // Dummy timer, must match `quadTransitionStartTime` below and `_bounceSpheresDelay`.
    {35.95, 35.95, {0}, {0}, LinearInterpolation, &_dummyKeyVal},
};

static fw_keyframe _keysBounceSpheresLogoSync[] = {
    {0, 0.2, {0,90}, {0,70}, QuadraticEaseOut, &_mountainsLogoPos},
    {0.2, 0.8, {0,70}, {0,90}, BounceEaseOut, &_mountainsLogoPos},
};

static const float __kl=12.f;
static fw_keyframe _keysLove[] = {
    {0, 2.25, _loveTransitionQuadPosInit, {0,0,-90}, QuadraticEaseIn, &_loveTransitionQuadPos},
    {6.25, 12.5, {0,0,-90}, _loveTransitionQuadPosInit, QuadraticEaseOut, &_loveTransitionQuadPos},

    {0, 4.25, _loveTransitionQuadRotInit, {.2,.7,.8,4*360}, QuadraticEaseIn, &_loveTransitionQuadRot},
    {4.25, 8.25, {.2,.7,.8,2*360}, {0,.0,.8,6*360}, LinearInterpolation, &_loveTransitionQuadRot},
    {8.25, 12.5, {1,.5,.8,6*360}, {0,0,0,8*360}, QuadraticEaseOut, &_loveTransitionQuadRot},

    {10.5, 10.5, {1}, {1}, LinearInterpolation, &_loveTransitionStencilEnabled},

    // `__kl` has to correspond with `FX_LOVE_TEXT_DELAY_SECONDS` and `FX_LOVE_RAINBOW_DELAY_SECONDS` in `love.c`.
    {__kl+0.5, __kl+0.5, {0}, {0}, LinearInterpolation, &_loveTransitionEnabled},
    {__kl+15.2, __kl+18.8, {0}, {4.6}, LinearInterpolation, &_hexTilesProgress},

    {__kl+7, __kl+13, {0}, {-2.55}, BackEaseInOut, &_loveRainbowPos},

    {__kl+13.7, __kl+15.7, {-2.55}, {-8}, QuadraticEaseIn, &_loveRainbowPos},
    {__kl+13.7, __kl+17.7, {0}, {-230}, BackEaseInOut, &_loveTextPos},
    {__kl+16.7, __kl+16.7, {-500}, {-500}, LinearInterpolation, &_loveTextPos},
};

static fw_keyframe _keysGreets[] = {
    {1.55, 2.55, _greetsStencil2PosInit, {-50}, QuadraticEaseInOut, &_greetsStencil2Pos},
    {2.55, 3.55, _greetsStencil1PosInit, {-50}, QuadraticEaseInOut, &_greetsStencil1Pos},

    {0, 0, {4.6}, {4.6}, LinearInterpolation, &_hexTilesProgress}, // Only needed if Greets anim is started directly.
    {26.8, 30.4, {4.6}, {0}, LinearInterpolation, &_hexTilesProgress},

    {27.3, 28.3, {FW_RENDER_WIDTH}, {FW_RENDER_WIDTH/2}, QuadraticEaseOut, &_greetsTransitQuadDim},
    {28.3, 29.3, {FW_RENDER_WIDTH/2}, {3*FW_RENDER_WIDTH/4}, QuadraticEaseInOut, &_greetsTransitQuadDim},
    {29.3, 30.3, {3*FW_RENDER_WIDTH/4}, {0}, QuadraticEaseInOut, &_greetsTransitQuadDim},

    // Artificial wait.
    {33.9, 33.9, {0}, {0}, LinearInterpolation, &_dummyKeyVal},
};

static const fw_vec4f __isoLogoBgColor1 = {23/255.f     , 32/255.f     , 56/255.f};
static const fw_vec4f __isoLogoBgColor2 = {.25*23/255.f , .25*32/255.f , .25*56/255.f};
static const fw_vec4f __isoLogoBgColor3 = {2.25*23/255.f, 2.25*32/255.f, 2.25*56/255.f};
static fw_keyframe _keysIsoPattern[] = {
    {0, 4, {50}, {-600}, SineEaseOut, &_isoLogoPos},
    {4, 18, {-600}, {1700}, SineEaseInOut, &_isoLogoPos},
    {18, 21, {1700}, {20 + 3*(FX_ISO_PATTERN_NUM_COLS*27)}, QuadraticEaseInOut, &_isoLogoPos},

    // Hack to simulate acceleration behavior.
    {4, 8, {0}, {300}, QuadraticEaseInOut, &_isoLogoPos2},
    {16, 20, {300}, {0}, SineEaseInOut, &_isoLogoPos2},

    {4, 9, {0}, {40}, QuadraticEaseInOut, &_isoLogoAmp},
    {11, 17, {40}, {10}, QuadraticEaseInOut, &_isoLogoAmp},
    {22, 25, {10}, {0}, QuadraticEaseInOut, &_isoLogoAmp},

    {20.5, 21.5, {166}, {290}, QuadraticEaseIn, &_isoLogoVerticalSpread},

    {21.5, 22.5, {0}, {1}, QuadraticEaseInOut, &_isoLogoPercentage},

    {22.5, 22.5, {FX_ISO_LOGO_FINAL_Y, FX_ISO_LOGO_MAX_Y}, {FX_ISO_LOGO_FINAL_Y, FX_ISO_LOGO_MAX_Y}, LinearInterpolation, &_isoLogoSrcDst},
    {22.3, 23.3, {0}, {255}, LinearInterpolation, &_isoLogoOutlineAlpha},

    {22.5, 23.5, {1}, {0}, QuadraticEaseInOut, &_isoLogoPercentage},

    {23, 23.5, _isoLogoShadowPosInit, {_isoLogoShadowPosInit.x, 244}, QuadraticEaseOut, &_isoLogoShadowPos},

    // BG colors.
    {7, 9, {0}, __isoLogoBgColor1, QuadraticEaseInOut, &_isoPatternBGColor},

    {9, 10, __isoLogoBgColor1, __isoLogoBgColor2, QuadraticEaseInOut, &_isoPatternBGColor},
    {10, 11, __isoLogoBgColor2, __isoLogoBgColor1, QuadraticEaseInOut, &_isoPatternBGColor},

    {11, 12, __isoLogoBgColor1, __isoLogoBgColor3, QuadraticEaseInOut, &_isoPatternBGColor},
    {12, 13, __isoLogoBgColor3, __isoLogoBgColor1, QuadraticEaseInOut, &_isoPatternBGColor},

    {13, 14, __isoLogoBgColor1, __isoLogoBgColor2, QuadraticEaseInOut, &_isoPatternBGColor},
    {14, 15, __isoLogoBgColor2, __isoLogoBgColor1, QuadraticEaseInOut, &_isoPatternBGColor},

    {16, 17, __isoLogoBgColor1, __isoLogoBgColor2, QuadraticEaseInOut, &_isoPatternBGColor},
    {17, 18, __isoLogoBgColor2, __isoLogoBgColor1, QuadraticEaseInOut, &_isoPatternBGColor},

    // BG gradient.
    {18, 19, {0}, {255}, QuadraticEaseInOut, &_isoPatternGradientAlpha},
};

static fw_keyframe _keysIsoLogoTransition[] = {
    // Artificial wait.
    {6, 6, {0}, {0}, LinearInterpolation, &_dummyKeyVal},
};

static fw_keyframe _keysDotTunnel[] = {
    {1, 3, _dotTunnelAmpInit, {.75}, LinearInterpolation, &_dotTunnelAmp},
    {11, 18, {0.75}, {1.25, 1.75}, LinearInterpolation, &_dotTunnelAmp},
    {25, 32, {1.25, 1.75}, {2.5}, LinearInterpolation, &_dotTunnelAmp},
    {36.25, 46.25, {2.5}, {0}, LinearInterpolation, &_dotTunnelAmp},

    {15.7, 17.7, _dotTunnelOverlayScaleInit, {3.1}, LinearInterpolation, &_dotTunnelOverlayScale},

    // TEXT 1
    {5, 7, _dotTunnelText3dPosInits[0], {-3, 1.2, -15}, QuadraticEaseOut, &_dotTunnelText3dAnimParams[0].pos},
    {11, 13, {-3, 1.2, -15}, {-1.5, 1.2, 3}, QuadraticEaseIn, &_dotTunnelText3dAnimParams[0].pos},
    {5, 6.5, {0}, {8}, LinearInterpolation, &_dotTunnelText3dAnimParams[0].palIndex},
    {12, 12.5, {8}, {0}, LinearInterpolation, &_dotTunnelText3dAnimParams[0].palIndex},

    // TEXT 2
    {7, 9, _dotTunnelText3dPosInits[1], {0, 1.2, -15}, QuadraticEaseOut, &_dotTunnelText3dAnimParams[1].pos},
    {12, 14, {0, 1.2, -15}, {0, 1.2, 3}, QuadraticEaseIn, &_dotTunnelText3dAnimParams[1].pos},
    {7, 8.5, {0}, {8}, LinearInterpolation, &_dotTunnelText3dAnimParams[1].palIndex},
    {13, 13.5, {8}, {0}, LinearInterpolation, &_dotTunnelText3dAnimParams[1].palIndex},

    // TEXT 3
    {9, 11, _dotTunnelText3dPosInits[2], {3, 1.2, -15}, QuadraticEaseOut, &_dotTunnelText3dAnimParams[2].pos},
    {13, 15, {3, 1.2, -15}, {1.5, 1.2, 3}, QuadraticEaseIn, &_dotTunnelText3dAnimParams[2].pos},
    {9, 10.5, {0}, {8}, LinearInterpolation, &_dotTunnelText3dAnimParams[2].palIndex},
    {14, 14.5, {8}, {0}, LinearInterpolation, &_dotTunnelText3dAnimParams[2].palIndex},

    // TEXT 4
    {11, 13, _dotTunnelText3dPosInits[3], {0, -0.2, -15}, QuadraticEaseOut, &_dotTunnelText3dAnimParams[3].pos},
    {14, 16, {0, -0.2, -15}, {1.1, 0.1, 3}, QuadraticEaseIn, &_dotTunnelText3dAnimParams[3].pos},
    {11, 12.5, {0}, {8}, LinearInterpolation, &_dotTunnelText3dAnimParams[3].palIndex},
    {15, 15.5, {8}, {0}, LinearInterpolation, &_dotTunnelText3dAnimParams[3].palIndex},

    {0, 1, _dotTunnelBorderTopPosInit, {70}, QuadraticEaseInOut, &_dotTunnelBorderTopPos},
    {1, 2, {70}, {100}, QuadraticEaseInOut, &_dotTunnelBorderTopPos},
    {2, 3, {100}, {40}, QuadraticEaseInOut, &_dotTunnelBorderTopPos},

    {42.25, 42.95, {40}, {60}, QuadraticEaseInOut, &_dotTunnelBorderTopPos},
    {42.95, 43.55, {60}, {15}, QuadraticEaseInOut, &_dotTunnelBorderTopPos},
    {43.55, 44.85, {15}, {270}, QuadraticEaseInOut, &_dotTunnelBorderTopPos},

    {43.55, 43.55, {1}, {1}, LinearInterpolation, &_dotTunnelIsBorderTopFG},

    {0, 1, _dotTunnelBorderBottomPosInit, {70}, QuadraticEaseInOut, &_dotTunnelBorderBottomPos},
    {1, 2, {70}, {100}, QuadraticEaseInOut, &_dotTunnelBorderBottomPos},
    {2, 3, {100}, {40}, QuadraticEaseInOut, &_dotTunnelBorderBottomPos},

    {3, 3, {0}, {0}, QuadraticEaseInOut, &_isoPatternGradientAlpha},
};

static fw_keyframe _keysCylinderTwistTransition[] = {
    {0, 0, {0}, {23/255.f,32/255.f,56/255.f,1}, LinearInterpolation, &_globalClearColor},

    {7, 11, {0}, {100, 0, 200, 120}, LinearInterpolation, &_cylinderOverlayColor},

    {3, 5, {.32, -.132}, {-.1875, -.132}, BackEaseInOut, &_cylinderCheckerRowPositions[0]},
    {5, 7, {-.1875, -.132}, {-.1875, .1875}, BackEaseInOut, &_cylinderCheckerRowPositions[0]},
    {7, 7.5, _cylinderCheckerRowFlashColor, _cylinderCheckerRowFinalColor, LinearInterpolation, &_cylinderCheckerRowColors[0]},

    {5.1, 7.1, {-.57, -.132}, {-.0625, -.132}, BackEaseInOut, &_cylinderCheckerRowPositions[1]},
    {7.1, 9.0, {-.0625, -.132}, {-.0625, .0625}, BackEaseInOut, &_cylinderCheckerRowPositions[1]},
    {9.0, 9.5, _cylinderCheckerRowFlashColor, _cylinderCheckerRowFinalColor, LinearInterpolation, &_cylinderCheckerRowColors[1]},
    {7.8, 8.7, {1, 0, 0, 180}, {0}, QuadraticEaseInOut, &_cylinderCheckerRowRotations[1]},

    {7.3, 9.3, {.32, -.132}, {-.1875, -.132}, BackEaseInOut, &_cylinderCheckerRowPositions[2]},
    {9.3, 10.9, {-.1875, -.132}, {-.1875, -.0625}, BackEaseInOut, &_cylinderCheckerRowPositions[2]},
    {10.9, 11.4, _cylinderCheckerRowFlashColor, _cylinderCheckerRowFinalColor, LinearInterpolation, &_cylinderCheckerRowColors[2]},
    {10.0, 10.7, {0, 0, 1, 90}, {0}, QuadraticEaseInOut, &_cylinderCheckerRowRotations[2]},

    {9.6, 11.6, {-.57, -.1875}, {-.0625, -.1875}, BackEaseInOut, &_cylinderCheckerRowPositions[3]},
    {11.8, 12.3, _cylinderCheckerRowFlashColor, _cylinderCheckerRowFinalColor, LinearInterpolation, &_cylinderCheckerRowColors[3]},
};

static fw_keyframe _keysCylinderTwist[] = {
    {0, 0, {0}, {23/255.f,32/255.f,56/255.f,1}, LinearInterpolation, &_globalClearColor},
    {0, 0, {0}, {100,0,200,120}, LinearInterpolation, &_cylinderOverlayColor},

    // Timings have to correspond with `initCylinderTexts`.

    {0, 1, _cylinderTwistSpeedInit, {0,-3}, LinearInterpolation, &_cylinderCheckerSpeed},
    {7.4, 9, _cylinderCheckerRowFinalColor, _cylinderCheckerRowFinalColorDimmed, QuadraticEaseInOut, &_cylinderCheckerGroundLayerColor},
    {17, 20, _cylinderCheckerRowFinalColorDimmed, _cylinderCheckerRowFinalColorDimmed2, QuadraticEaseInOut, &_cylinderCheckerGroundLayerColor},

    {9, 11, _cylinderYDeltasInit, {0,-6,-6}, QuadraticEaseInOut, &_cylinderYDeltas},
    {13, 15, {0,-6,-6}, {0,0,-6}, QuadraticEaseInOut, &_cylinderYDeltas},
    {19, 22, {0,0,-6}, {0,0,0}, QuadraticEaseInOut, &_cylinderYDeltas},
    {34.42, 38.42, {0,0,0}, {-6,0,0}, QuadraticEaseInOut, &_cylinderYDeltas},
    {38.42, 41.42, {-4.5,0,0}, {2.8f,0,0}, QuadraticEaseInOut, &_cylinderYDeltas},
    {62.82, 67.82, {2.8f,0,0}, {2.8f,12,12}, QuadraticEaseInOut, &_cylinderYDeltas},
    {66.82, 69.82, {2.8f,12,12}, {9,12,12}, QuadraticEaseInOut, &_cylinderYDeltas},

    // Transform to big red cylinder.
    {38.42, 38.42, _cylinderTwistSlicesInit, {8,4,4}, QuadraticEaseInOut, &_cylinderTwistSlices},
    {38.42, 38.42, _cylinderTwistBaseRadiusInit, {.6,.3,.3}, QuadraticEaseInOut, &_cylinderTwistBaseRadius},
    {38.42, 38.42, _cylinderTwistTopRadiusInit, {.6,.3,.3}, QuadraticEaseInOut, &_cylinderTwistTopRadius},
    {38.42, 38.42, _cylinderTwistYSineAmpInit, {0,2,2.5}, QuadraticEaseInOut, &_cylinderTwistYSineAmp},

    // Shrink both the small cyliners.
    {38.42, 40.42, {.6,.3,.3}, {.6,.1,.1}, QuadraticEaseInOut, &_cylinderTwistBaseRadius},
    {38.42, 40.42, {.6,.3,.3}, {.6,.1,.1}, QuadraticEaseInOut, &_cylinderTwistTopRadius},

    // Bottom base of two small cylinders.
    {62.82, 64.82, {.6,.1,.1}, {.6,.02,.02}, QuadraticEaseInOut, &_cylinderTwistBaseRadius},

    // Bottom base of big cylinder.
    {64.82, 66.82, {.6,.01,.01}, {.0,.02,.02}, QuadraticEaseInOut, &_cylinderTwistBaseRadius},

    // Distance when yellow enters.
    {26, 28, _cylinderTwistDistanceInit, {.4,0}, QuadraticEaseInOut, &_cylinderTwistDistance},
    {30, 32, {.4,0}, {1.0,.5}, QuadraticEaseInOut, &_cylinderTwistDistance},
    {32, 37, {1.0,.5}, _cylinderTwistDistanceInit, QuadraticEaseInOut, &_cylinderTwistDistance},


    {38.42, 40.42, _cylinderTwistDistanceInit, {.6,0}, QuadraticEaseInOut, &_cylinderTwistDistance},
    {43.42, 45.42, {.6,0}, {.4,0}, QuadraticEaseInOut, &_cylinderTwistDistance},

    // Distance between big/small cylinders.
    {48, 49, {.4,0}, {.9,0}, QuadraticEaseInOut, &_cylinderTwistDistance},
    {49, 51, {.9,0}, {.4,0}, QuadraticEaseInOut, &_cylinderTwistDistance},

    {51, 52, {.4,0}, {1,0}, QuadraticEaseInOut, &_cylinderTwistDistance},
    {52, 54, {1,0}, {.4,0}, QuadraticEaseInOut, &_cylinderTwistDistance},

    {54, 55, {.4,0}, {.8,0}, QuadraticEaseInOut, &_cylinderTwistDistance},
    {55, 62, {.8,0}, {.4,0}, QuadraticEaseInOut, &_cylinderTwistDistance},

    // Variance when yellow enters.
    {24, 25, _cylinderTwistVarianceInit, {.003}, QuadraticEaseInOut, &_cylinderTwistVariance},
    {26, 29, {.003}, _cylinderTwistVarianceInit, QuadraticEaseInOut, &_cylinderTwistVariance},

    {38.42, 40.42, _cylinderTwistVarianceInit, {.0025f}, QuadraticEaseInOut, &_cylinderTwistVariance},
    {54.82, 56.82, {.0025f}, {.005f}, QuadraticEaseInOut, &_cylinderTwistVariance},

    {2, 4, _cylinderCheckerZoomInit, {0}, QuadraticEaseInOut, &_cylinderCheckerZoom},
    {7, 9, {0}, {-1}, QuadraticEaseInOut, &_cylinderCheckerZoom},
    {10, 13, {-1}, {-.5}, QuadraticEaseInOut, &_cylinderCheckerZoom},
    {13, 16, {-.5}, {-1}, QuadraticEaseInOut, &_cylinderCheckerZoom},
    {17, 20, {-1}, {-2}, QuadraticEaseInOut, &_cylinderCheckerZoom},
    {58.82, 64.82, {-2}, {-1}, QuadraticEaseInOut, &_cylinderCheckerZoom},

    {8, 9, _cylinderCheckerMiddelSpreadInit, {1}, QuadraticEaseOut, &_cylinderCheckerMiddelSpread},

    {65.57, 67.57, {0}, {-FW_RENDER_HEIGHT}, QuadraticEaseOut, &_cylinderTwistTransitionOutQuadHeight}
};

static fw_keyframe _keysEndBlindsTransition[] = {
    {0, 0, {0}, {122/255.f,54/255.f,123/255.f,255/255.f}, LinearInterpolation, &_globalClearColor},

    // Has to match `renderEndBlindsTransitionScene`.
    {3.5, 3.5, {0}, {0}, LinearInterpolation, &_dummyKeyVal},
};

static fw_keyframe _keysEndScroller[] = {
    {0, 0, {0}, {76/255.f,44/255.f,49/255.f,255/255.f}, LinearInterpolation, &_globalClearColor},
    {95, 95, {0}, {0}, LinearInterpolation, &_globalClearColor},

    {0, 1.5f, {1,0,1,0}, {1,0,1,FW_RENDER_HEIGHT}, QuadraticEaseOut, &_endScrollerLineDims[0]},
    {0, 1.5f, {FW_RENDER_WIDTH-2,FW_RENDER_HEIGHT,1,0}, {FW_RENDER_WIDTH-2,FW_RENDER_HEIGHT,1,-FW_RENDER_HEIGHT}, QuadraticEaseOut, &_endScrollerLineDims[1]},

    {92, 94, {0}, {FW_RENDER_HEIGHT}, QuadraticEaseInOut, &_creditsBGHeight},

    {96, 96, {0}, {0}, LinearInterpolation, &_dummyKeyVal},
};


///////////////////////////////////////////////////////////////////////////////
// ANIMS
///////////////////////////////////////////////////////////////////////////////

static fw_animation _anims[] = {
    [ANIM_PRESENTS] = {
        .isAutostart = 1,
        .keyframes = _keysPresents,
        .numKeyframes = LEN(_keysPresents),
        .started_proc = animStartedPresents,
        .completed_proc = animCompletedPresents,
    },
    [ANIM_PRESENTS_TEXT1] = {
        .isAutostart = 1,
    },
    [ANIM_TITLE_LOGO] = {
        .isAutostart = 0,
        .keyframes = _keysTitleLogo,
        .numKeyframes = LEN(_keysTitleLogo),
        .started_proc = animStartedTitleLogo,
        .completed_proc = animCompletedTitleLogo,
    },
    [ANIM_ENDLESS_GRID] = {
        .isAutostart = 0,
        .keyframes = _keysEndlessGrid,
        .numKeyframes = LEN(_keysEndlessGrid),
        .started_proc = animStartedEndlessGrid,
        .completed_proc = animCompletedEndlessGrid,
    },
    [ANIM_TWIRL_SPHERE] = {
        .isAutostart = 0,
        .keyframes = _keysTwirlSphere,
        .numKeyframes = LEN(_keysTwirlSphere),
        .started_proc = animStartedTwirlSphere,
        .completed_proc = animCompletedTwirlSphere,
    },
    [ANIM_MOUNTAINS_SCROLLER] = {
        .isAutostart = 0,
        .keyframes = _keysMountainsScroller,
        .numKeyframes = LEN(_keysMountainsScroller),
        .started_proc = animStartedMountainsScroller,
        .completed_proc = animCompletedMountainsScroller,
    },
    [ANIM_BOUNCE_SPHERES] = {
        .isAutostart = 0,
        .keyframes = _keysBounceSpheres,
        .numKeyframes = LEN(_keysBounceSpheres),
        .started_proc = animStartedBounceSpheres,
        .completed_proc = animCompletedBounceSpheres,
    },
    [ANIM_BOUNCE_SPHERES_LOGO_SYNC] = {
        .isAutostart = 0,
        .keyframes = _keysBounceSpheresLogoSync,
        .numKeyframes = LEN(_keysBounceSpheresLogoSync),
    },
    [ANIM_LOVE] = {
        .isAutostart = 0,
        .keyframes = _keysLove,
        .numKeyframes = LEN(_keysLove),
        .started_proc = animStartedLove,
        .completed_proc = animCompletedLove,
    },
    [ANIM_GREETS] = {
        .isAutostart = 0,
        .keyframes = _keysGreets,
        .numKeyframes = LEN(_keysGreets),
        .started_proc = animStartedGreets,
        .completed_proc = animCompletedGreets,
    },
    [ANIM_ISO_PATTERN] = {
        .isAutostart = 0,
        .keyframes = _keysIsoPattern,
        .numKeyframes = LEN(_keysIsoPattern),
        .started_proc = animStartedIsoPattern,
        .completed_proc = animCompletedIsoPattern,
    },
    [ANIM_ISO_PATTERN_IN] = {
        .isAutostart = 0,
    },
    [ANIM_ISO_PATTERN_OUT] = {
        .isAutostart = 0,
    },
    [ANIM_ISO_LOGO_TRANSITION] = {
        .isAutostart = 0,
        .keyframes = _keysIsoLogoTransition,
        .numKeyframes = LEN(_keysIsoLogoTransition),
        .started_proc = animStartedIsoLogoTransition,
        .completed_proc = animCompletedIsoLogoTransition,
    },
    [ANIM_ISO_LOGO_TRANSITION_LINE] = {
        .isAutostart = 0,
    },
    [ANIM_DOT_TUNNEL] = {
        .isAutostart = 0,
        .keyframes = _keysDotTunnel,
        .numKeyframes = LEN(_keysDotTunnel),
        .started_proc = animStartedDotTunnel,
        .completed_proc = animCompletedDotTunnel,
    },
    [ANIM_CYLINDER_TWIST_TRANSITION] = {
        .isAutostart = 0,
        .keyframes = _keysCylinderTwistTransition,
        .numKeyframes = LEN(_keysCylinderTwistTransition),
        .started_proc = animStartedCylinderTwistTransition,
        .completed_proc = animCompletedCylinderTwistTransition,
    },
    [ANIM_CYLINDER_TWIST_TEXT1] = {
        .isAutostart = 0,
    },
    [ANIM_CYLINDER_TWIST_TEXT2] = {
        .isAutostart = 0,
    },
    [ANIM_CYLINDER_TWIST] = {
        .isAutostart = 0,
        .keyframes = _keysCylinderTwist,
        .numKeyframes = LEN(_keysCylinderTwist),
        .started_proc = animStartedCylinderTwist,
        .completed_proc = animCompletedCylinderTwist,
    },
    [ANIM_END_BLINDS_TRANSITION] = {
        .isAutostart = 0,
        .keyframes = _keysEndBlindsTransition,
        .numKeyframes = LEN(_keysEndBlindsTransition),
        .started_proc = animStartedEndBlindsTransition,
        .completed_proc = animCompletedEndBlindsTransition,
    },
    [ANIM_END_SCROLLER] = {
        .isAutostart = 0,
        .keyframes = _keysEndScroller,
        .numKeyframes = LEN(_keysEndScroller),
        .started_proc = animStartedEndScroller,
        .completed_proc = animCompletedEndScroller,
    },
};

static fw_animation_schedule _animSchedule = {
    _anims,
    LEN(_anims)
};

static BOOL isRunning(enum anim_names_enum animNameEnum) {
    return _animSchedule.animations[animNameEnum].state == RUNNING;
}


///////////////////////////////////////////////////////////////////////////////
// ANIM START / STOP IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////

static void animStartedPresents(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedPresents(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_TITLE_LOGO], time);
}

static void animStartedTitleLogo(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedTitleLogo(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_ENDLESS_GRID], time);
}

static void animStartedEndlessGrid(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedEndlessGrid(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_TWIRL_SPHERE], time);
}

static void animStartedTwirlSphere(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedTwirlSphere(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_MOUNTAINS_SCROLLER], time);
}

static void animStartedMountainsScroller(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedMountainsScroller(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_BOUNCE_SPHERES], time);
}

static void animStartedBounceSpheres(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedBounceSpheres(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_LOVE], time);
}

static void animStartedLove(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedLove(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_GREETS], time);
}

static void animStartedGreets(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedGreets(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_ISO_PATTERN], time);
    fw_animation_start(&_anims[ANIM_ISO_PATTERN_IN], time);
    fw_animation_start(&_anims[ANIM_ISO_PATTERN_OUT], time);
}

static void animStartedIsoPattern(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedIsoPattern(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_ISO_LOGO_TRANSITION], time);
    fw_animation_start(&_anims[ANIM_ISO_LOGO_TRANSITION_LINE], time);
}

static void animStartedIsoLogoTransition(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedIsoLogoTransition(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_DOT_TUNNEL], time);
}


static void animStartedDotTunnel(fw_animation *anim, fw_timer_data *time) {
}

static void animCompletedDotTunnel(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_CYLINDER_TWIST_TRANSITION], time);
    fw_animation_start(&_anims[ANIM_CYLINDER_TWIST_TEXT1], time);
    fw_animation_start(&_anims[ANIM_CYLINDER_TWIST_TEXT2], time);
}

static void animStartedCylinderTwistTransition(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedCylinderTwistTransition(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_CYLINDER_TWIST], time);
}

static void animStartedCylinderTwist(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedCylinderTwist(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_END_BLINDS_TRANSITION], time);
}

static void animStartedEndBlindsTransition(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedEndBlindsTransition(fw_animation *anim, fw_timer_data *time) {
    fw_animation_start(&_anims[ANIM_END_SCROLLER], time);
}

static void animStartedEndScroller(fw_animation *anim, fw_timer_data *time) {
}
static void animCompletedEndScroller(fw_animation *anim, fw_timer_data *time) {
    fw_broadcast_exit();
}


///////////////////////////////////////////////////////////////////////////////
// MUSIC SYNC
///////////////////////////////////////////////////////////////////////////////
static int _syncEndlessGridTitleFlashAlpha = 0;
static int _syncEndlessGridTitle2FlashAlpha = 0;
static int _syncEndlessGridTitle3FlashAlpha = 0;
static float _syncEndlessGridBGAlphaAdd = 0;

static float _syncTwirlShperePointSizeAdd = 0;

static float _syncMountainsSunScale = 1;
static float _syncMountainsShadowScale = 1;
static float _syncMountainsLogoFlashAlpha = 0;

static float _syncBounceSphereIntensity = .3f;
static float _syncBounceSphereFloorIntensity = 0;

static float _syncGreetsEvenPosX = 0;
static float _syncGreetsEvenSpeedX = 0;
static float _syncGreetsOddPosX = 0;
static float _syncGreetsOddSpeedX = 0;
static const float _syncGreetsAccelX = .15*.6;
static const float _syncGreetsDecayX = 2.3;

static int _syncIsoLogoFlashAlpha = 0;
static int _syncIsoPatternFlashAlpha = 0;

static int _syncDotTunnelBGFlashAlpha = 0;

static fw_vec3f _syncCylinderCheckerGroundLayerColor;
static fw_vec3f _syncCylinderCheckerMiddleLayerColor;
static fw_vec3f _syncCylinderCheckerTopLayerColor;
static fw_vec3f _syncCylinderColorAdd;

static float _syncLoveFlash = 0;

static void triggerSyncKick(int beatNumber, fw_timer_data *time) {
    if (isRunning(ANIM_BOUNCE_SPHERES)) {
        _syncBounceSphereIntensity = .9f;

        if (_bounceSphere.center.x < 4.5f) {
            _syncMountainsLogoFlashAlpha = 170;
            _syncBounceSphereFloorIntensity = 255;
            fw_animation_start(&_anims[ANIM_BOUNCE_SPHERES_LOGO_SYNC], time);
        }
    }

    if (isRunning(ANIM_LOVE)
            && time->elapsed < 165) {
        _syncLoveFlash = 1;
    }

    if (isRunning(ANIM_CYLINDER_TWIST)
        && time->elapsed > 327 ) {
        _syncCylinderColorAdd.x = 255-225;
        _syncCylinderColorAdd.y = 255-50;
        _syncCylinderColorAdd.z = 255-60;
    }

    if (isRunning(ANIM_GREETS)) {
        _syncGreetsOddSpeedX = _syncGreetsAccelX;
    }
}

static void triggerSyncSnare(int beatNumber, fw_timer_data *time) {
    if (isRunning(ANIM_GREETS)) {
        _syncGreetsEvenSpeedX = _syncGreetsAccelX;
    }

    if (isRunning(ANIM_DOT_TUNNEL)
        && time->elapsed > 246) {
        _syncDotTunnelBGFlashAlpha = 50;
    }

    if (isRunning(ANIM_CYLINDER_TWIST)) {
        if (time->elapsed < 298) {
            _syncCylinderCheckerGroundLayerColor.x = 38;
            _syncCylinderCheckerGroundLayerColor.y = 130;
            _syncCylinderCheckerGroundLayerColor.z = 105;
        } else if (time->elapsed < 307)  {
            _syncCylinderCheckerMiddleLayerColor.x = 87;
            _syncCylinderCheckerMiddleLayerColor.y = 26;
            _syncCylinderCheckerMiddleLayerColor.z = 107;
        } else {
            _syncCylinderCheckerTopLayerColor.x = 38;
            _syncCylinderCheckerTopLayerColor.y = 130;
            _syncCylinderCheckerTopLayerColor.z = 105;
        }
    }
}

static void triggerSyncKickOrSnare(int beatNumber, fw_timer_data *time) {
    if (isRunning(ANIM_TWIRL_SPHERE)
            && time->elapsed > 61.7
            && time->elapsed < 72.2 ) {
        _syncTwirlShperePointSizeAdd = 12;
    }

    if (isRunning(ANIM_MOUNTAINS_SCROLLER)
            && time->elapsed > 79
            && time->elapsed < 96.9) {
        _syncMountainsSunScale = 1.4f;
        _syncMountainsShadowScale = 1.3f;
    }
}

static void triggerIsoPatternSync(int beatNumber) {
    if ( beatNumber == 3432
            || beatNumber == 3560) {
        _syncIsoPatternFlashAlpha = 255;
    }

    if (beatNumber == 3688 + 4*2) {
        _syncIsoLogoFlashAlpha = 255;
    }
}

static void triggerEndlessGridSync(int beatNumber) {
    for (int i=0; i<_endlessGridRowNoteMapLength; i++) {
        if (_endlessGridRowNoteMap[i].rowNum <= beatNumber) {
            int idx = _endlessGridRowNoteMap[i].note;
            _endlessGridStencilStates[idx] = TRUE;
        }
    }

    for (int i=0; i<_endlessGridRowNoteMapLength; i++) {
        if (_endlessGridRowNoteMap[i].rowNum == beatNumber) {
            int idx = _endlessGridRowNoteMap[i].note;
            _endlessGridSliceAlphas[idx] = 255;
            _syncEndlessGridBGAlphaAdd = .05f;
        }
    }

    if (beatNumber == _endlessGridRowNoteMap[0].rowNum-64
            || beatNumber == _endlessGridRowNoteMap[1].rowNum-64
            || beatNumber == _endlessGridRowNoteMap[3].rowNum-64) {
        _syncEndlessGridBGAlphaAdd = .05f;
    }

    if (beatNumber > 128+656) {
        _title2Pos.y = -100;
    }
    if (beatNumber == 128+656) {
        _syncEndlessGridTitle2FlashAlpha = 255*4;
    }

    if (beatNumber > 128+656+4) {
        _title3Pos.y = -100;
    }
    if (beatNumber == 128+656+4) {
        _syncEndlessGridTitle3FlashAlpha = 255*4;
    }

    if (beatNumber > 128+656+8) {
        _titlePos.y = -100;
    }
    if (beatNumber == 128+656+8) {
        _syncEndlessGridTitleFlashAlpha = 255*4;
    }

}

static void triggerEndScrollerSync(int beatNumber) {
    for (int i=0; i<_endScrollerRowNoteMapLength; i++) {
        if (_endScrollerRowNoteMap[i].rowNum == beatNumber) {
            int idx = _endScrollerRowNoteMap[i].note;
            _endScrollerRowColors[idx].w = 255;
        }
    }
}

static char _isKickSnareInverted = 1;
static void triggerSync(int beatNumber, fw_timer_data *time) {
    int isKick = beatNumber % 16 == 0;
    int isSnare = (beatNumber+8) % 16 == 0;

    if (isRunning(ANIM_ENDLESS_GRID)) {
        triggerEndlessGridSync(beatNumber);
    }
    if (isRunning(ANIM_ISO_PATTERN)) {
        triggerIsoPatternSync(beatNumber);
    }
    if (isRunning(ANIM_DOT_TUNNEL)) {
        // After ANIM_ISO_PATTERN is over, snare and kick are switched again in the song positions.
        _isKickSnareInverted = 0;
    }
    if (isRunning(ANIM_END_SCROLLER)) {
        triggerEndScrollerSync(beatNumber);
    }

    if (_isKickSnareInverted) {
        int isKickOrig = isKick;
        isKick = isSnare;
        isSnare = isKickOrig;
    }

    if (isKick) {
        triggerSyncKick(beatNumber, time);
    }

    if (isSnare) {
        triggerSyncSnare(beatNumber, time);
    }

    if (isKick || isSnare) {
        triggerSyncKickOrSnare(beatNumber, time);
    }
}

///////////////////////////////////////////////////////////////////////////////
// INIT
///////////////////////////////////////////////////////////////////////////////

static void initIsoPattern(void) {
    fx_isologo_init();

    _isoLogoMeta.numOffsets = FX_ISO_PATTERN_NUM_COLS;
    _isoLogoMeta.patternVerticalSpread = 166;

    const float offsetY = 50;
    for (int i=0; i<_isoLogoMeta.numOffsets; i++) {
        _isoLogoMeta.offsets[i].y = offsetY + 450;
    }

    const int numKeyframes = FX_ISO_PATTERN_NUM_COLS;

    _anims[ANIM_ISO_PATTERN_IN].numKeyframes = numKeyframes;
    _anims[ANIM_ISO_PATTERN_IN].keyframes = calloc(numKeyframes, sizeof(fw_keyframe));

    _anims[ANIM_ISO_PATTERN_OUT].numKeyframes = numKeyframes;
    _anims[ANIM_ISO_PATTERN_OUT].keyframes = calloc(numKeyframes, sizeof(fw_keyframe));

    float delay = .7;
    for (int i=0; i<numKeyframes; i++) {
        fw_keyframe *key = &_anims[ANIM_ISO_PATTERN_IN].keyframes[i];
        key->timeStart = delay + i*.15;
        key->timeEnd = delay + i*.15+3;
        key->from = (fw_vec4f) {
            0, offsetY + 450
        };
        key->to = (fw_vec4f) {
            0, offsetY
        };
        key->easing_func = ExponentialEaseOut;
        key->value = &_isoLogoMeta.offsets[ i ];
    }

    // Shuffeled from 0..17 (see FX_ISOLOGO_NUM_PATTERN_COLS). Start with 0.
    const int shuffeledIdx[] = {0,14,6,4,3,13,10,16,2,1,8,17,12,15,7,9,5,11};
    delay = 17;
    for (int i=0; i<numKeyframes; i++) {
        fw_keyframe *key = &_anims[ANIM_ISO_PATTERN_OUT].keyframes[i];
        key->timeStart = delay + i*.125;
        key->timeEnd = delay + i*.125+2;
        key->from = (fw_vec4f) {
            0, offsetY
        };
        key->to = (fw_vec4f) {
            0, offsetY + 580 *  (i%2==0?-1:1)
        };
        key->easing_func = QuadraticEaseIn;
        key->value = &_isoLogoMeta.offsets[ shuffeledIdx[i] ];
    }

    // Column with index 0 must stay in place. Set "to" value to the same value as "from".
    fw_keyframe *key = &_anims[ANIM_ISO_PATTERN_OUT].keyframes[0];
    key->to = (fw_vec4f) {
        0, offsetY
    };
}

static void initDotTunnel3dTexts() {
    fx_text_3d_params *tp;
    text_3d_anim_params *ap;
    int idx = 0;

    // TEXT 1
    tp = &_dotTunnelTextParams[idx];
    tp->text = "DRIVE";
    tp->fontFace = getFontFace();
    tp->scale = .04f;
    tp->isPaletteEnabled = TRUE;
    tp->palette = _dotTunnelText3dPalette;
    tp->paletteIndex = 0;

    ap = &_dotTunnelText3dAnimParams[idx];
    ap->pos =_dotTunnelText3dPosInits[idx];
    ap->sineAmp = (fw_vec4f) {
        .25f,.15f,2.f
    };
    ap->sineOffset = (fw_vec4f) {
        1,0,1
    };
    ap->sineSpeed = (fw_vec4f) {
        2*1,2*.75f,2*1
    };

    // TEXT 2
    idx++;
    tp = &_dotTunnelTextParams[idx];
    tp->text = "INTO";
    tp->fontFace = getFontFace();
    tp->scale = .04f;
    tp->isPaletteEnabled = TRUE;
    tp->palette = _dotTunnelText3dPalette;
    tp->paletteIndex = 0;

    ap = &_dotTunnelText3dAnimParams[idx];
    ap->pos =_dotTunnelText3dPosInits[idx];
    ap->sineAmp = (fw_vec4f) {
        .25f,.15f,2.f
    };
    ap->sineOffset = (fw_vec4f) {
        0,1,1
    };
    ap->sineSpeed = (fw_vec4f) {
        2*1,2*.75f,2*1
    };

    // TEXT 3
    idx++;
    tp = &_dotTunnelTextParams[idx];
    tp->text = "THE";
    tp->fontFace = getFontFace();
    tp->scale = .04f;
    tp->isPaletteEnabled = TRUE;
    tp->palette = _dotTunnelText3dPalette;
    tp->paletteIndex = 0;

    ap = &_dotTunnelText3dAnimParams[idx];
    ap->pos =_dotTunnelText3dPosInits[idx];
    ap->sineAmp = (fw_vec4f) {
        .25f,.15f,2.f
    };
    ap->sineOffset = (fw_vec4f) {
        1,1,0
    };
    ap->sineSpeed = (fw_vec4f) {
        2*1,2*.75f,2*1
    };

    // TEXT 4
    idx++;
    tp = &_dotTunnelTextParams[idx];
    tp->text = "COLORFUL!";
    tp->fontFace = getFontFace();
    tp->scale = .08f;
    tp->isPaletteEnabled = TRUE;
    tp->palette = _dotTunnelText3dPalette;
    tp->paletteIndex = 0;

    ap = &_dotTunnelText3dAnimParams[idx];
    ap->pos =_dotTunnelText3dPosInits[idx];
    ap->sineAmp = (fw_vec4f) {
        .35f,.25f,.2f
    };
    ap->sineOffset = (fw_vec4f) {
        3,2,0
    };
    ap->sineSpeed = (fw_vec4f) {
        2,.5f,1
    };
}

static void initIsoLogoLineTransition(void) {
    _isoLogoTransitionLinePositions = calloc(_numIsoLogoTransitionLinePositions, sizeof(fw_vec4f));

    for (int i=0; i<_numIsoLogoTransitionLinePositions; i++) {
        _isoLogoTransitionLinePositions[i].y = -2;
    }

    _anims[ANIM_ISO_LOGO_TRANSITION_LINE].numKeyframes = _numIsoLogoTransitionLinePositions;
    _anims[ANIM_ISO_LOGO_TRANSITION_LINE].keyframes = calloc(_numIsoLogoTransitionLinePositions, sizeof(fw_keyframe));

    srand(2);
    const float startDelay = 0;
    const float keyframeDelay = .025f;
    const float duration = 2;
    for (int i=0; i<_numIsoLogoTransitionLinePositions; i++) {
        const float d = fw_math_randf(0,100);
        const float b = fw_math_randf(0,1.2);
        const float h = fw_math_randf(2,100);

        fw_keyframe *key = &_anims[ANIM_ISO_LOGO_TRANSITION_LINE].keyframes[i];
        key->timeStart = startDelay + d * keyframeDelay;
        key->timeEnd = startDelay + d * keyframeDelay + duration + b;
        key->from = (fw_vec4f) {
            0,-h
        }; // Height determines how quick/hard to bounce.
        key->to = (fw_vec4f) {
            0,FW_RENDER_HEIGHT/2
        };
        key->easing_func = BounceEaseOut;
        key->value = &_isoLogoTransitionLinePositions[i];
    }
}

fw_keyframe createAnimTextKey(fx_anim_text_meta *meta, int keyType, int totalKeyIdx, int totalCharIdx, int lineIdx, int charIdx) {
    fw_keyframe key = {0};

    const float totalDelay = meta->delay;
    const float lineDurations[] = {3, 4, 3};
    const float lineDelays[] =    {0, 5, 12};

    const float timeOffset = lineDelays[lineIdx] + 0.1*charIdx;
    const float rD = fw_math_randf(0,1);
    const float rH = fw_math_randf(10,20);
    const int rS = fw_math_rand(0,1) > 0 ? 1:-1;

    const float yoff = lineIdx * 20;
    // Define x-offsets for every textline in _presentsText1.
    const float xoffs[] = {129,85,100};

    switch(keyType) {
    case 0:
        // Color index hack. Hold palette index. -1 results in fully transparent text.
        // See: renderPresentsScene
        key.from = (fw_vec4f) {
            -1
            };
        key.to = (fw_vec4f) {
            22
        };
        key.timeStart = totalDelay + 0.f + timeOffset;
        key.timeEnd   = totalDelay + 1.f + timeOffset;
        key.easing_func = QuadraticEaseIn;
        key.value = &meta->modifiers[lineIdx][charIdx].color;
        break;
    case 1:
        // Translate.
        key.from = (fw_vec4f) {
            xoffs[lineIdx], (int)(rH*rS - yoff)
        };
        key.to = (fw_vec4f) {
            xoffs[lineIdx], -yoff
        };
        key.timeStart = totalDelay + 0.f + timeOffset;
        key.timeEnd   = totalDelay + 1.f + timeOffset + rD;
        key.easing_func = QuadraticEaseOut;
        key.value = &meta->modifiers[lineIdx][charIdx].translate;
        break;
    case 2:
        // Color index hack.
        key.from = (fw_vec4f) {
            22
        };
        key.to = (fw_vec4f) {
            -1
            };
        key.timeStart = totalDelay + lineDurations[lineIdx] + .5f + timeOffset;
        key.timeEnd   = totalDelay + lineDurations[lineIdx] + 1.f + timeOffset;
        key.easing_func = QuadraticEaseOut;
        key.value = &meta->modifiers[lineIdx][charIdx].color;
        break;
    case 3:
        // Translate.
        key.from = (fw_vec4f) {
            xoffs[lineIdx], -yoff
        };
        key.to = (fw_vec4f) {
            xoffs[lineIdx], (int)(rH*rS - yoff)
        };
        key.timeStart = totalDelay + lineDurations[lineIdx] + 0.f + timeOffset;
        key.timeEnd   = totalDelay + lineDurations[lineIdx] + 1.f + timeOffset + rD;
        key.easing_func = QuadraticEaseIn;
        key.value = &meta->modifiers[lineIdx][charIdx].translate;
        break;
    }

    return key;
}

static void initPresentsTexts(void) {
    //Good: 24, 40
    srand(40);
    _presentsTextMeta1.strings = _presentsText1;
    _presentsTextMeta1.numLines = _numLinesPresentsText1;
    _presentsTextMeta1.numKeysPerChar = 4;
    _presentsTextMeta1.delay = 5.5;
    _presentsTextMeta1.duration = 0; // unused.
    _presentsTextMeta1.createKey = createAnimTextKey;
    fx_anim_text_init_meta(&_presentsTextMeta1);
    _anims[ANIM_PRESENTS_TEXT1].keyframes = _presentsTextMeta1.keys;
    _anims[ANIM_PRESENTS_TEXT1].numKeyframes = _presentsTextMeta1.numKeys;

    // Init color palette hack. Set x component of color to palette entry -1.
    // In `renderPresentsScene` this gets treated as fully transparent.
    for (int i=0; i<_presentsTextMeta1.numLines; i++) {
        for (int j=0; j<strlen(_presentsTextMeta1.strings[i]); j++) {
            _presentsTextMeta1.modifiers[i][j].color.x=-1;
        }
    }
}

static void initCylinderText(fw_vec4f lineColorIndexes[], enum anim_names_enum animNameEnum, float totalDelay, float pauseDuration, int lineDelays[]) {
    // Every line has its current palette index stored here.
    //*lineColorIndexes = calloc(NUM_CYLINDER_TEXT_LINES, sizeof(fw_vec4f));

    for (int i=0; i<NUM_CYLINDER_TEXT_LINES; i++) {
        lineColorIndexes[i].x = -1;
    }

    const int numKeys = NUM_CYLINDER_TEXT_LINES * 2; // Every line has a fade in and fade out.
    _anims[animNameEnum].numKeyframes = numKeys;
    _anims[animNameEnum].keyframes = calloc(numKeys, sizeof(fw_keyframe));

    const float delay = .1f;
    const float duration = .75f;
    for (int j=0; j<numKeys; j++) {
        fw_keyframe *k = &_anims[animNameEnum].keyframes[j];
        k->timeStart = totalDelay + lineDelays[j] * delay;
        k->timeEnd = totalDelay + lineDelays[j] * delay + duration;
        k->from = (fw_vec4f) {
            _presentsPaletteSize-1
        };
        k->to = (fw_vec4f) {
            -1
            };
        k->easing_func=LinearInterpolation;
        k->value=&lineColorIndexes[j % NUM_CYLINDER_TEXT_LINES];
    }
    // Middle line stays at max index.
    _anims[animNameEnum].keyframes[NUM_CYLINDER_TEXT_LINES/2].to = (fw_vec4f) {
        _presentsPaletteSize-1
    };

    // Wait time to fade out.
    for (int j=numKeys/2; j<numKeys; j++) {
        fw_keyframe *k = &_anims[animNameEnum].keyframes[j];
        k->timeStart += pauseDuration;
        k->timeEnd += pauseDuration;
    }
}

static void initCylinderTexts(void) {
    const float duration1 = 13;

    int lineDelaysDownThenCenter[] = {
        0,1,2,3,4,5,6,7,8,9,10,11,12, // Fade in line delays.
        6,5,4,3,2,1,0,1,2,3,4,5,6 // Fade out line delays.
    };
    initCylinderText(_cylinderText1PaletteIndexes, ANIM_CYLINDER_TWIST_TEXT1, 1, duration1, lineDelaysDownThenCenter);


    int lineDelaysCenterThenCenter[] = {
        0,1,2,3,4,5,6,5,4,3,2,1,0, // Fade in line delays.
        6,5,4,3,2,1,0,1,2,3,4,5,6 // Fade out line delays.
    };
    initCylinderText(_cylinderText2PaletteIndexes, ANIM_CYLINDER_TWIST_TEXT2, duration1 + 2.3, 4, lineDelaysCenterThenCenter);
}

static void copyPattern(sync_row_note *dst, const sync_row_note *src, int srcLen, int dstIdxOffset, int patternOffset) {
    for (int i=0; i<srcLen; i++) {
        dst[i+dstIdxOffset].rowNum = src[i].rowNum + patternOffset;
        dst[i+dstIdxOffset].note = src[i].note;
    }
}

static void initEndScrollerMusicSync() {
    // Note 0 = F-4
    const sync_row_note pattern1[] = {
        { 0*2, 5}, // A#-4
        { 4*2, 7}, // C-5
        { 6*2, 9}, // D-5
        {12*2, 9}, // D-5
        {13*2, 7}, // C-5
        {14*2, 0}, // F-4
        {15*2, 2}, // G-4
        {16*2, 5}, // A#-4
        {20*2, 7}, // C-5
        {22*2, 9}, // D-5
        {29*2, 9}, // D-5
        {30*2, 7}, // C-5
        {31*2, 9}, // D-5
        {32*2, 5}, // A#-4
        {36*2, 7}, // C-5
        {38*2, 9}, // D-5
        {44*2, 7}, // C-5
        {48*2, 12}, // F-5
        {50*2, 12}, // F-5
        {53*2, 9}, // D-5
        {54*2, 11}, // E-5
        {58*2, 12}, // F-5
        {60*2, 11}, // E-5
        {61*2, 9}, // D-5
        {62*2, 7}, // C-5
    };
    const int pattern1Length = LEN(pattern1);

    const sync_row_note pattern2[] = {
        { 0*2, 5}, // A#-4
        { 4*2, 7}, // C-5
        { 6*2, 9}, // D-5
        {12*2, 9}, // D-5
        {13*2, 7}, // C-5
        {14*2, 0}, // F-4
        {15*2, 2}, // G-4
        {16*2, 5}, // A#-4
        {20*2, 7}, // C-5
        {22*2, 9}, // D-5
        {29*2, 9}, // D-5
        {30*2, 7}, // C-5
        {31*2, 9}, // D-5
        {32*2, 5}, // A#-4
        {36*2, 7}, // C-5
        {38*2, 9}, // D-5
        {44*2, 9}, // D-5
        {45*2, 7}, // C-5
        {46*2, 5}, // A#-4
        {47*2, 2}, // G-4
        {50*2, 5}, // A#-4
        {54*2, 6}, // B-4
        {55*2, 7}, // C-5
    };
    const int pattern2Length = LEN(pattern2);

    _endScrollerRowNoteMapLength = 4*(pattern1Length + pattern2Length); // 4x both patterns.
    _endScrollerRowNoteMap = calloc(_endScrollerRowNoteMapLength, sizeof(sync_row_note));

    // Pattern offset = 64 * n:
    // 64: full pattern length
    // 2: sync is set to speed 03, but this part of the song has speed 06.

    const int startRow = 6080; // Absolute row number of end song start.
    int idx = 0;
    copyPattern(_endScrollerRowNoteMap, pattern1, pattern1Length, idx, startRow + 64*0);
    idx += pattern1Length;
    copyPattern(_endScrollerRowNoteMap, pattern2, pattern2Length, idx, startRow + 64*2);
    idx += pattern2Length;
    copyPattern(_endScrollerRowNoteMap, pattern1, pattern1Length, idx, startRow + 64*4);
    idx += pattern1Length;
    copyPattern(_endScrollerRowNoteMap, pattern2, pattern2Length, idx, startRow + 64*6);
    idx += pattern2Length;

    copyPattern(_endScrollerRowNoteMap, pattern1, pattern1Length, idx, startRow + 64*12);
    idx += pattern1Length;
    copyPattern(_endScrollerRowNoteMap, pattern2, pattern2Length, idx, startRow + 64*14);
    idx += pattern2Length;
    copyPattern(_endScrollerRowNoteMap, pattern1, pattern1Length, idx, startRow + 64*16);
    idx += pattern1Length;
    copyPattern(_endScrollerRowNoteMap, pattern2, pattern2Length, idx, startRow + 64*18);
}

void scene_0_init() {
    fw_animation_resetAnimationSchedule(&_animSchedule);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    srand(1);

    fx_dot_tunnel_init(&_dotTunnel, 96,48,.1f,.5f,.5f);
    _dotTunnelDefaultColors = _dotTunnel.dotColors;
    _dotTunnelRainbowColors = fx_dot_tunnel_createColorsDefault(_dotTunnel.numDotsPerRing);
    fx_dot_tunnel_initRainbowColors(_dotTunnelRainbowColors, _dotTunnel.numDotsPerRing);
    _dotTunnel.dotColors = _dotTunnelRainbowColors;

    // Init dot spheres.
    fx_dot_sphere_init();

    fx_dot_sphere_initSphere(&_bounceSphere, 1);
    _bounceSphere.velocity.x = -3.5;
    _bounceSphere.center.x = 4.8;

    fx_dot_sphere_initSphere(&_twirlSphere, 1);

    // Lighting.
    glShadeModel(GL_FLAT);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat[]) {
        -2, 2, 5, 0
        });

    fx_cylinder_twist_init();
    fx_cylinder_twist_initParams(&_cylinderParams);

    fx_endless_grid_init();

    initIsoPattern();
    initIsoLogoLineTransition();
    initDotTunnel3dTexts();

    initPresentsTexts();
    initCylinderTexts();

    // Set up music sync: 125 BPM, 3 SPEED. Trigger every row.
    fw_sync_init2(&_syncData, 125, 3, 1, triggerSync);
    initEndScrollerMusicSync();
}


///////////////////////////////////////////////////////////////////////////////
// EXECUTE
///////////////////////////////////////////////////////////////////////////////

static fw_timer_data sceneTime(enum anim_names_enum animNameEnum, fw_timer_data *time) {
    fw_timer_data sceneTime = *time;
    sceneTime.elapsed = time->elapsed - _animSchedule.animations[animNameEnum].timeStartedAt;
    return sceneTime;
}

static void color_black(void) {
    glColor3ub(0,0,0);
}

static void color_white(void) {
    glColor3ub(255,255,255);
}

// +++++++++++++++ //
// SCENE: PRESENTS
// +++++++++++++++ //

static void renderPresentsScene(fw_timer_data *time) {
    color_black();
    fw_quad_put(0, _presentsBGPos.y + 80*sinf(time->elapsed*2), FW_RENDER_WIDTH, FW_RENDER_HEIGHT*2);

    // Palette hack: color.x denotes the palette index.
    for (int i=0; i<_presentsTextMeta1.numLines; i++) {
        for (int j=0; j<strlen(_presentsTextMeta1.strings[i]); j++) {
            int palIdx = _presentsTextMeta1.modifiers[i][j].color.x;
            if (palIdx<0) {
                _presentsTextMeta1.modifiers[i][j].color.w=0;
            } else {
                _presentsTextMeta1.modifiers[i][j].color.x=_presentsPalette[palIdx].x/255.f;
                _presentsTextMeta1.modifiers[i][j].color.y=_presentsPalette[palIdx].y/255.f;
                _presentsTextMeta1.modifiers[i][j].color.z=_presentsPalette[palIdx].z/255.f;
                _presentsTextMeta1.modifiers[i][j].color.w=1;
            }
        }
    }
    fx_anim_text_render(&_presentsTextMeta1, getFontFace(), 0,125, 1, NULL);
}

// ++++++++++++ //
// SCENE: TITLE
// ++++++++++++ //

static void renderIsoPatternBackground(float alpha) {
    fw_image_renderBegin(&getImages()[RES_IMG_TITLE_BG], alpha);
    fw_image_putImage(&getImages()[RES_IMG_TITLE_BG]);
    fw_image_renderEnd();
}

static float calcFlickerAlpha(fw_timer_data *time) {
    const float alpha = 95 + 4.f*sinf(64*time->elapsed) + 6.f*sinf(1.f+16*time->elapsed);
    return alpha / 255.f;
}

static void renderTitleBG() {
    fw_stencil_begin();

    const float mH = 10;
    const float mY = _titleBgPos.y;

    fw_stencil_activateStencilBuffer();

    renderIsoPatternBackground(1);

    fw_stencil_activateColorBuffer(0);

    // Middle.
    color_white();
    glTranslatef(0,mY,0);
    fw_quad_put(0,0,FW_RENDER_WIDTH,mH);

    // Top bayer border.
    fx_overlay_renderOverlayExt(RES_IMG_BAYER_HORIZONTAL, 255,255,255,255, 0,mY, 1,-1, 44,0, 1,0);

    // Bottom bayer border.
    fx_overlay_renderOverlayExt(RES_IMG_BAYER_HORIZONTAL, 255,255,255,255, 0,mH+mY, 1,1, 44,0, 1,0);

    fw_stencil_end();
}

static void renderTitleLogoPart(enum ImagesEnum imageEnum, float x, float y, float brightness, float alpha) {
    fw_image_renderBegin(&getImages()[imageEnum], 1);
    glTranslatef(x,y,0);
    glColor4f(brightness,brightness,brightness, alpha);
    fw_image_putImage(&getImages()[imageEnum]);
    fw_image_renderEnd();
}

static void renderTitleLogoParts(float mainBrightness, float mainAlpha) {
    renderTitleLogoPart(RES_IMG_TITLE_2, _title2Pos.x,_title2Pos.y, mainBrightness, mainAlpha);
    renderTitleLogoPart(RES_IMG_TITLE, _titlePos.x,_titlePos.y, mainBrightness, mainAlpha);
    renderTitleLogoPart(RES_IMG_TITLE_3, _title3Pos.x,_title3Pos.y, mainBrightness, mainAlpha);
}

static void renderTitleLogoPartsWithFlicker(fw_timer_data *time) {
    // Title with brightness flicker.
    renderTitleLogoParts( (120 + 10.f*sinf(32*time->elapsed+1) + 20.f*sinf(2.f+8*time->elapsed))/255.f, 1 );

    // Overlay bright flicker.
    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_NOTEQUAL, 0);
    float alpha = 2*calcFlickerAlpha(time);
    renderTitleLogoPart(RES_IMG_TITLE_2, _title2Pos.x,_title2Pos.y, 1, alpha);
    renderTitleLogoPart(RES_IMG_TITLE, _titlePos.x,_titlePos.y, 1, alpha);
    renderTitleLogoPart(RES_IMG_TITLE_3, _title3Pos.x,_title3Pos.y, 1, alpha);

    glDisable(GL_ALPHA_TEST);
    fw_render_defaultBlendMode();

    // Overlay flash out.
    if (_syncEndlessGridTitleFlashAlpha > 0) {
        _syncEndlessGridTitleFlashAlpha = fw_math_lerp(_syncEndlessGridTitleFlashAlpha, 0, 16*time->delta);
        renderTitleLogoPart(RES_IMG_TITLE_MASK, _titlePosInit.x, _titlePosInit.y, 1, _syncEndlessGridTitleFlashAlpha/255.f);
    }

    if (_syncEndlessGridTitle2FlashAlpha > 0) {
        _syncEndlessGridTitle2FlashAlpha = fw_math_lerp(_syncEndlessGridTitle2FlashAlpha, 0, 16*time->delta);
        renderTitleLogoPart(RES_IMG_TITLE_2_MASK, _title2PosInit.x, _title2PosInit.y, 1, _syncEndlessGridTitle2FlashAlpha/255.f);
    }

    if (_syncEndlessGridTitle3FlashAlpha > 0) {
        _syncEndlessGridTitle3FlashAlpha = fw_math_lerp(_syncEndlessGridTitle3FlashAlpha, 0, 16*time->delta);
        renderTitleLogoPart(RES_IMG_TITLE_3_MASK, _title3PosInit.x, _title3PosInit.y, 1, _syncEndlessGridTitle3FlashAlpha/255.f);
    }
}

static void renderTitleLogo(fw_timer_data *time) {
    fw_projection_ortho();

    // Outline.
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        const float outlineStencilH = 70;
        glPushMatrix();
        {
            color_black();
            glTranslatef(0,_titleOutlineStencilPos.y,0);
            fw_quad_put(0,0,FW_RENDER_WIDTH,outlineStencilH);
        }
        glPopMatrix();

        fw_image_renderBegin(&getImages()[RES_IMG_BAYER_HORIZONTAL], 1);
        glTranslatef(0,(int)(outlineStencilH + _titleOutlineStencilPos.y),0);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_HORIZONTAL], 44,0, 1,0);

        // Slightly translate bayer pattern to omit too much flickering.
        glTranslatef(2,1,0);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_HORIZONTAL], 44,0, 1,0);
        fw_image_renderEnd();

        fw_stencil_activateColorBuffer(0);

        fw_image_renderBegin(&getImages()[RES_IMG_TITLE_2_OUTLINE], 1);
        glTranslatef(126,97,0);
        fw_image_putImage(&getImages()[RES_IMG_TITLE_2_OUTLINE]);
        fw_image_renderEnd();

        fw_image_renderBegin(&getImages()[RES_IMG_TITLE_OUTLINE], 1);
        glTranslatef(4,111,0);
        fw_image_putImage(&getImages()[RES_IMG_TITLE_OUTLINE]);
        fw_image_renderEnd();
    }
    fw_stencil_end();

    // Color.
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        glPushMatrix();
        {
            color_white();
            glTranslatef(0,_titleStencilPos.y,0);
            fw_quad_put(0,0,FW_RENDER_WIDTH,270);
        }
        glPopMatrix();

        fw_image_renderBegin(&getImages()[RES_IMG_BAYER_HORIZONTAL], 1);
        glTranslatef(0,1+_titleStencilPos.y,0);
        glScalef(1,-1,1);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_HORIZONTAL], 44,0, 1,0);
        fw_image_renderEnd();

        fw_stencil_activateColorBuffer(0);

        const float alpha = calcFlickerAlpha(time);
        renderIsoPatternBackground(alpha);

        renderTitleLogoPartsWithFlicker(time);
    }
    fw_stencil_end();

    // Reflection.
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        glPushMatrix();
        {
            color_white();
            glTranslatef(_titleRefelectionPos.x,80,0);
            glRotatef(45,0,0,1);
            fw_quad_put(10,0,10,140);
        }
        glPopMatrix();

        fw_stencil_activateColorBuffer(0);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        renderTitleLogoParts(1,.75f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    fw_stencil_end();
}

static void renderTitleScene(fw_timer_data *time) {
    renderTitleBG();
    renderTitleLogo(time);
}

// +++++++++++++++++++ //
// SCENE: ENDLESS GRID
// +++++++++++++++++++ //

static void renderEndlessGridBorder(void) {
    // Top/Bottom Border.
    fw_projection_ortho();
    for (int i=0; i<LEN(_endlessGridBorderDims); i++) {
        color_white();
        fw_quad_put(_endlessGridBorderDims[i].x,
                    _endlessGridBorderDims[i].y,
                    _endlessGridBorderDims[i].z,
                    _endlessGridBorderDims[i].w);
    }
}

static void updateEndlessGridSceneSync(fw_timer_data *time) {
    const float dt = 8*time->delta;

    _syncEndlessGridBGAlphaAdd = fw_math_lerp(_syncEndlessGridBGAlphaAdd, 0, dt);

    // Update the slice flashes that appear synced to the beat.
    for (int i=0; i<_endlessGridRowNoteMapLength; i++) {
        if (_endlessGridStencilStates[i]) {
            _endlessGridSliceAlphas[i] = fw_math_lerp(_endlessGridSliceAlphas[i], 0, dt);
        }
    }
}

static void renderEndlessGridBGScene(void) {
    if (_syncEndlessGridBGAlphaAdd > 0) {
        fw_render_clearHiRes(_globalClearColor.x + _syncEndlessGridBGAlphaAdd,
                             _globalClearColor.y + _syncEndlessGridBGAlphaAdd,
                             _globalClearColor.z + _syncEndlessGridBGAlphaAdd,
                             _globalClearColor.w);
    }
}

static void renderEndlessGridScene(fw_timer_data *time) {
    updateEndlessGridSceneSync(time);

    const float alpha = _titleBgAlpha.x * calcFlickerAlpha(time) + 1.5f*_syncEndlessGridBGAlphaAdd;
    renderIsoPatternBackground(alpha);

    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        fw_projection_ortho();
        color_white();

        // _endlessGridBorderStencilDim.z is the indicator that the big stencil is to be used and not the slices.
        if (_endlessGridBorderStencilDim.z > 0) {
            fw_quad_put(_endlessGridBorderStencilDim.x,
                        _endlessGridBorderStencilDim.y,
                        _endlessGridBorderStencilDim.z,
                        _endlessGridBorderStencilDim.w);
        } else {
            for (int i=0; i<_endlessGridRowNoteMapLength; i++) {
                if (_endlessGridStencilStates[i]) {
                    fw_quad_put(_endlessGridSliceDims[i].x,
                                _endlessGridSliceDims[i].y,
                                _endlessGridSliceDims[i].z,
                                _endlessGridSliceDims[i].w);
                }
            }
        }

        fw_stencil_activateColorBuffer(0);
        // Checker background.
        color_black();
        fw_quad_put(0,0,FW_RENDER_WIDTH, FW_RENDER_HEIGHT);

        fw_projection_perspective();
        fx_endless_grid_update(time);

        if (_endlessGridColorAlpha.x> 0) {
            fx_endless_grid_render(getImages()[RES_IMG_CHECKER_DOT].textureID, FALSE, _endlessGridColorAlpha.x, time);
        }

        if (_endlessGridMonoAlpha.x > 0) {
            fx_endless_grid_render(getImages()[RES_IMG_CHECKER_DOT].textureID, TRUE, _endlessGridMonoAlpha.x, time);
        }

        if (_endlessGridBorderStencilDim.z <= 0) {
            renderEndlessGridBorder();
        }
    }
    fw_stencil_end();

    // Render the slice flashes that appear synced to the beat.
    fw_projection_ortho();
    for (int i=0; i<_endlessGridRowNoteMapLength; i++) {
        if (_endlessGridStencilStates[i]) {
            glColor4ub(255,255,255, _endlessGridSliceAlphas[i]);
            fw_quad_put(_endlessGridSliceDims[i].x,
                        _endlessGridSliceDims[i].y,
                        _endlessGridSliceDims[i].z,
                        _endlessGridSliceDims[i].w);
        }
    }

    if (_endlessGridBorderStencilDim.z > 0) {
        renderEndlessGridBorder();
    }

    renderTitleLogoPartsWithFlicker(time);
}

// +++++++++++++++++++ //
// SCENE: TWIRL SPHERE
// +++++++++++++++++++ //

static void renderTwirlSphereBgTop() {
    glPushMatrix();
    glColor3ub(255, 132, 38);
    fw_quad_put(0,5,FW_RENDER_WIDTH, 1);
    fw_quad_put(0,7,FW_RENDER_WIDTH, 2);
    fw_quad_put(0,10,FW_RENDER_WIDTH, FW_RENDER_HEIGHT);
    glPopMatrix();
    fx_overlay_renderOverlay(RES_IMG_GRADIENT_GROUND, 100,0,200,100, 0,0, 3,3);
}

static void renderTwirlSphereBgBottom() {
    glPushMatrix();
    glColor3ub(156,156,156);
    fw_quad_put(0,153,FW_RENDER_WIDTH, 1);
    fw_quad_put(0,159,FW_RENDER_WIDTH, 1);
    fw_quad_put(0,164,FW_RENDER_WIDTH, 1);
    fw_quad_put(0,168,FW_RENDER_WIDTH, 1);
    fw_quad_put(0,170,FW_RENDER_WIDTH, 100);
    glPopMatrix();

    fx_overlay_renderOverlay(RES_IMG_GRADIENT_GROUND, 255, 255, 255, 100, 0,332, 3,-2);
}

static void renderTwirlSphereTrellisTop() {
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        glPushMatrix();
        color_black();
        fw_quad_put(_twirlSphereTrellisTopPos.x,0,FW_RENDER_WIDTH,FW_RENDER_HEIGHT);
        glPopMatrix();

        fw_stencil_activateColorBuffer(0);
        fw_image_renderBegin(&getImages()[RES_IMG_TRELLIS], 1);
        glColor4ub(255, 255, 255, 100);
        glTranslatef(11,102,0);
        glScalef(1,-1,1);
        fw_image_putImage(&getImages()[RES_IMG_TRELLIS]);
        fw_image_renderEnd();
    }
    fw_stencil_end();
}

static void renderTwirlSphereTrellisBottom() {
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        glPushMatrix();
        color_black();
        fw_quad_put(_twirlSphereTrellisBottomPos.x,0,FW_RENDER_WIDTH,FW_RENDER_HEIGHT);
        glPopMatrix();

        fw_stencil_activateColorBuffer(0);
        fw_image_renderBegin(&getImages()[RES_IMG_TRELLIS], 1);
        glColor4ub(0, 0, 0, 80);
        glTranslatef(11,178,0);
        fw_image_putImage(&getImages()[RES_IMG_TRELLIS]);
        fw_image_renderEnd();
    }
    fw_stencil_end();
}

static void renderTwirlSphere(fw_timer_data *time) {
    fw_projection_perspective();

    glPushMatrix();
    glTranslatef(0,_twirlSphereShadowPos.x,-4);
    fx_dot_sphere_renderShadow(&_twirlSphere, 1,1, -1.67f);
    glPopMatrix();

    glEnable(GL_SCISSOR_TEST);

    const int SCISSOR_HEIGHT = 2;
    const float t = time->elapsed;

    glPushMatrix();
    glTranslatef(0,_twirlSpherePos.x,-7);
    for (int i=0; i<FW_RENDER_HEIGHT; i+=SCISSOR_HEIGHT) {
        glScissor(0,i,FW_RENDER_WIDTH,SCISSOR_HEIGHT);

        glPushMatrix();
        glRotatef(t * 120 + i*.5, sinf(.01*i+t),cosf(.01*i+t),sinf(.01*i+t));
        glScalef(_twirlSphereScale.x,_twirlSphereScale.y,_twirlSphereScale.z);
        fx_dot_sphere_render(&_twirlSphere, 1,1, 2 + _syncTwirlShperePointSizeAdd);
        glPopMatrix();

    }
    glPopMatrix();
    glDisable(GL_SCISSOR_TEST);
}

static void updateTwirlSphereSync(fw_timer_data *time) {
    _syncTwirlShperePointSizeAdd = fw_math_lerp(_syncTwirlShperePointSizeAdd, 0, 8*time->delta);
}

static void renderTwirlSphereScene(fw_timer_data *time) {
    updateTwirlSphereSync(time);

    fw_projection_ortho();

    // Top BG.
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();
        glPushMatrix();
        color_white();
        fw_quad_put(0,0,FW_RENDER_WIDTH, _twirlSphereBgTop.x);
        glPopMatrix();

        fw_stencil_activateColorBuffer(0);
        renderTwirlSphereBgTop();
    }
    fw_stencil_end();
    renderTwirlSphereTrellisTop();

    // Bottom BG.
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();
        glPushMatrix();
        color_white();
        fw_quad_put(0,FW_RENDER_HEIGHT,FW_RENDER_WIDTH, _twirlSphereBgBottom.x);
        glPopMatrix();

        fw_stencil_activateColorBuffer(0);
        renderTwirlSphereBgBottom();
    }
    fw_stencil_end();
    renderTwirlSphereTrellisBottom();

    if (_twirlSphereScale.x <= 0) {
        renderEndlessGridBorder();
    } else {
        renderTwirlSphere(time);
    }
}

// ++++++++++++++++ //
// SCENE: MOUNTAINS
// ++++++++++++++++ //

static void updateMountainsSync(fw_timer_data *time) {
    _syncMountainsShadowScale = fw_math_lerp(_syncMountainsShadowScale, 1, time->delta*3);
    _syncMountainsSunScale = fw_math_lerp(_syncMountainsSunScale, 1, time->delta*3);
}

static void renderSunsetBG() {
    fw_projection_ortho();
    renderTwirlSphereBgTop();
    renderTwirlSphereBgBottom();
}

static void renderMountainsSun(void) {
    fw_image_renderBegin(&getImages()[RES_IMG_MOUNTAINS_SUN], 1);
    glColor3ub(255,247,244);

    glTranslatef(100,_mountainsSunPos.y,0);
    fw_image_scaleCenter(&getImages()[RES_IMG_MOUNTAINS_SUN], _syncMountainsSunScale);

    fw_image_putImage(&getImages()[RES_IMG_MOUNTAINS_SUN]);
    fw_image_renderEnd();
}

static void renderMountains(void) {
    fw_stencil_begin();

    fw_stencil_activateStencilBuffer();
    glPushMatrix();
    glColor3ub(0,0,0);
    fw_quad_put(0,0,FW_RENDER_WIDTH,96+74);
    glPopMatrix();

    fw_stencil_activateColorBuffer(0);
    fw_image_renderBegin(&getImages()[RES_IMG_MOUNTAINS], 1);
    glColor3ub(255,255,255);
    glTranslatef(-1,_mountainsPos.y,0);
    fw_image_putImage(&getImages()[RES_IMG_MOUNTAINS]);
    fw_image_renderEnd();

    fw_stencil_end();
}

static void renderMountainsShadow(void) {
    fw_stencil_begin();

    fw_stencil_activateStencilBuffer();
    fw_image_renderBegin(&getImages()[RES_IMG_MOUNTAINS], 1);
    color_black();
    glTranslatef(-1,94.5,0);
    glTranslatef(0,getImages()[RES_IMG_MOUNTAINS].height,0);
    glScalef(1, _mountainsShadowScale.y  * _syncMountainsShadowScale, 1);
    glTranslatef(0,-getImages()[RES_IMG_MOUNTAINS].height,0);
    fw_image_putImage(&getImages()[RES_IMG_MOUNTAINS]);
    fw_image_renderEnd();

    fw_stencil_activateColorBuffer(0);
    glPushMatrix();
    {
        glColor4ub(0,0,0,64);
        fw_quad_put(0,0,FW_RENDER_WIDTH,FW_RENDER_HEIGHT);
    }
    glPopMatrix();

    fw_stencil_end();
}

static void renderMountainsLogo(void) {
    fw_image_renderBegin(&getImages()[RES_IMG_MOUNTAINS_LOGO], 1);
    glColor4ub((unsigned char)_mountainsLogoColor.x,
               (unsigned char)_mountainsLogoColor.y,
               (unsigned char)_mountainsLogoColor.z,
               (unsigned char)_mountainsLogoColor.w);
    glTranslatef(60,_mountainsLogoPos.y,0);
    fw_image_putImage(&getImages()[RES_IMG_MOUNTAINS_LOGO]);
    fw_image_renderEnd();
}

static void renderMountainsTextShadow(void) {
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        fx_overlay_renderBayerBorderLeftRight(0,0, 255,255,255);

        fw_stencil_activateColorBuffer(1);

        const float s = .01;
        const float w = getImages()[RES_IMG_MOUNTAINS_TEXT].width * s;
        const float h = getImages()[RES_IMG_MOUNTAINS_TEXT].height * s;
        const float shadowH = h * 6.f * _syncMountainsShadowScale;

        fw_projection_perspective();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, getImages()[RES_IMG_MOUNTAINS_TEXT].textureID);

        glPushMatrix();
        {
            glTranslatef(_mountainsTextPos.x, _mountainsTextPos.y + .04f, _mountainsTextPos.z);
            glColor3ub(120, 120, 120);
            glTranslatef(0,-h,0);
            glRotatef(-80,1,0,0);
            glTranslatef(0,-shadowH,0);
            glScalef(w,-shadowH,0);
            fw_quad_putTextured(1,0, 1,0);
        }
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
    }
    fw_stencil_end();
}

static void renderMountainsScene(fw_timer_data *time) {
    updateMountainsSync(time);

    renderSunsetBG();
    renderMountainsSun();
    renderMountains();
    renderMountainsShadow();
    renderMountainsLogo();
    renderMountainsTextShadow();
}

static void renderMountainsTextScrollerScene(void) {
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        fx_overlay_renderBayerBorderLeftRight(0,0, 255,255,255);

        fw_stencil_activateColorBuffer(1);

        const float s = .01;
        const float w = getImages()[RES_IMG_MOUNTAINS_TEXT].width * s;
        const float h = getImages()[RES_IMG_MOUNTAINS_TEXT].height * s;

        fw_projection_perspective();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, getImages()[RES_IMG_MOUNTAINS_TEXT].textureID);
        glPushMatrix();
        {
            glColor3ub(60, 94, 139);
            glTranslatef(_mountainsTextPos.x, _mountainsTextPos.y, _mountainsTextPos.z);
            glScalef(w,h,0);
            fw_quad_putTextured(1,0, 1,0);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glColor3ub(162, 62, 140);
            glTranslatef(_mountainsTextPos.x, _mountainsTextPos.y, _mountainsTextPos.z+.05);
            glScalef(w,h,0);
            fw_quad_putTextured(1,0, 1,0);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glColor3ub(208, 218, 145);
            glTranslatef(_mountainsTextPos.x, _mountainsTextPos.y, _mountainsTextPos.z+.1);
            glScalef(w,h,0);
            fw_quad_putTextured(1,0, 1,0);
        }
        glPopMatrix();

        glPushMatrix();
        {
            color_white();
            glTranslatef(_mountainsTextPos.x, _mountainsTextPos.y, _mountainsTextPos.z+.15);
            glScalef(w,h,0);
            fw_quad_putTextured(1,0, 1,0);
        }
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
    }
    fw_stencil_end();
}

// +++++++++++++++++++++ //
// SCENE: BOUNCE SPHERES
// +++++++++++++++++++++ //

static void renderBounceSpheresBGParallax(fw_timer_data *time) {
    // Layer 1
    fw_image_renderBegin(&getImages()[RES_IMG_PARALLAX_LAYER_1], 1);
    glTranslatef(0,100,0);
    glColor3ub(64, 39, 81);
    fw_image_putImageExt(&getImages()[RES_IMG_PARALLAX_LAYER_1], 4, time->elapsed*.5f, 1, 0);
    fw_image_renderEnd();

    // Layer 2
    fw_image_renderBegin(&getImages()[RES_IMG_PARALLAX_LAYER_2], 1);
    glTranslatef(0,132,0);
    glColor3ub(122, 54, 123);
    fw_image_putImageExt(&getImages()[RES_IMG_PARALLAX_LAYER_2], 6, time->elapsed, 1, 0);
    fw_image_renderEnd();

    // Layer 3
    fw_image_renderBegin(&getImages()[RES_IMG_PARALLAX_LAYER_3], 1);
    glTranslatef(0,132+24,0);
    glColor3ub(162, 62, 140);
    fw_image_putImageExt(&getImages()[RES_IMG_PARALLAX_LAYER_3], 12, time->elapsed*3, 1, 0);
    fw_image_renderEnd();

    // Conceal bottom
    glColor3ub(162, 62, 140);
    fw_quad_put(0,165,FW_RENDER_WIDTH,120);
    glClear(GL_DEPTH_BUFFER_BIT);
}

static void renderBounceSpheresBGFloor(fw_timer_data *time) {
    glPushMatrix();
    {
        glTranslatef(0,-3.9,-6);
        glScalef(10,2,1.3);
        glColor3ub(122, 54, 123);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, getImages()[RES_IMG_STRIPE].textureID);
        glPushMatrix();
        {
            glScalef(1,1,4.5);
            glTranslatef(0,1,-1);
            glRotatef(90,1,0,0);
            fw_quad_putTextured(9,time->elapsed*2.5f, 3,0);
        }
        glPopMatrix();
        glPushMatrix();
        {
            fw_quad_putTextured(9,time->elapsed*2.5f, 3,.5);
        }
        glPopMatrix();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

static void renderBounceSpheresBGFloorFlash(fw_timer_data *time) {
    if (_syncBounceSphereFloorIntensity <= 0) {
        return;
    }

    fw_projection_perspective();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, getImages()[RES_IMG_BOUNCE_SPHERES_FLASH].textureID);

    //glColor4f(1,1,1,_syncColor);
    glColor4ub(122, 54, 123, _syncBounceSphereFloorIntensity);

    glTranslatef(0,0,-7);
    glTranslatef(_bounceSphere.center.x, -1.5, _bounceSphere.center.z);
    glScalef(7,.75,1);

    //fw_quad_putTextured(1,0,1,0);
    fw_quad_putTextured(1,0,1,0);
    fw_quad_putTextured(1,0,1,0);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    fw_render_defaultBlendMode();
}

static void renderBounceSpheresBGGradientOverlay() {
    fx_overlay_renderOverlay(RES_IMG_GRADIENT_GROUND, 0,0,0,100, 0,170, 3,1);
    fx_overlay_renderOverlay(RES_IMG_GRADIENT_GROUND, 0,0,0,100, 0,0, 3,2.6);
}

static void renderBounceSpheresBGScene(fw_timer_data *time) {
    renderBounceSpheresBGParallax(time);
    fw_projection_perspective();
    renderBounceSpheresBGFloor(time);
    renderBounceSpheresBGGradientOverlay();
    renderBounceSpheresBGFloorFlash(time);
}

static void renderBounceSpheresQuadTransition(fw_vec3f color, float startTime, float time) {
    fw_projection_ortho();
    glColor3f(color.x,color.y,color.z);

    const int numX = 16;
    const int numY = 12;
    const float t = time - startTime;
    const float w = FW_RENDER_WIDTH / (float)numX; // Quad width.
    const float h = FW_RENDER_HEIGHT / (float)numY; // Quad height.
    const unsigned char numWaves = 4; // Number of full cosine waves to process in each quad.
    float sx, sy;  // Quad x and y scale.

    for (int j=0; j<numY; j++) {
        for (int i=0; i<numX; i++) {
            const float freq = M_PI + 6*t*1.5 - i*2.84 - j*3.84;

            // Skip if first wave has not been passed.
            if (freq < M_PI ) {
                continue;
            }

            if (freq > numWaves*M_PI ) {
                // Wave limit has been reached. Scale quad to full width/height.
                sx = w;
                sy = h;
            } else {
                // Else calculate scale factor depending on freq value.
                const float cosFreq = cosf(freq);
                sx = w/2.f + w/2.f * cosFreq;
                sy = h/2.f + h/2.f * cosFreq;
            }

            glPushMatrix();
            glTranslatef(w + i*w*2, h + j*h*2,0);
            glScalef(sx,sy,1);
            fw_quad_put(-1,-1, 2, 2);
            glPopMatrix();
        }
    }
}

static void renderBounceSpheresBayerQuad(float xPos, unsigned char r, unsigned char g, unsigned char b) {
    fw_image_renderBegin(&getImages()[RES_IMG_BAYER_VERTICAL], 1);
    {
        glColor3ub(r,g,b);
        glTranslatef(FW_RENDER_WIDTH-xPos,0,0);
        fw_quad_put(0,0,FW_RENDER_WIDTH, FW_RENDER_HEIGHT);

        glPushMatrix();
        glTranslatef(FW_RENDER_WIDTH,0,0);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_VERTICAL], 1,0, 34,0);
        glPopMatrix();

        glScalef(-1,1,1);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_VERTICAL], 1,0, 34,0);

    }
    fw_image_renderEnd();
}

static void updateBounceSphereSync(fw_timer_data *time) {
    _syncBounceSphereIntensity = fw_math_lerp(_syncBounceSphereIntensity, 0.0f, time->delta);
    _syncMountainsLogoFlashAlpha = fw_math_lerp(_syncMountainsLogoFlashAlpha, 0.0f, 2*time->delta);
    _syncBounceSphereFloorIntensity = fw_math_lerp(_syncBounceSphereFloorIntensity, 0, 2*time->delta);
}


static void updateBounceSphere(fw_timer_data *time) {
    const float horizontalBoundary = 3.5;
    _bounceSphere.center.x += time->delta * _bounceSphere.velocity.x;
    if ( (_bounceSphere.velocity.x > 0 && _bounceSphere.center.x > horizontalBoundary)
            || (_bounceSphere.velocity.x < 0 && _bounceSphere.center.x < -horizontalBoundary)) {
        _bounceSphere.velocity.x *= -1;
        _bounceSphere.rotation.z *= -1;
        _bounceSphere.rotation.x *= -1;
    }
}


static void renderBounceSphere(fw_timer_data *globalTime) {
    const float t = globalTime->elapsed;
    const float bounceFreq = .48f; // 1 row is 0.06s. Every kick is 8 * 0.06 = 0.48.
    const float bounceHeight = 3.0;
    _bounceSphere.center.y = -.9f + bounceHeight * fabs(cosf(t * M_PI / (2 * bounceFreq) ));
    _bounceSphere.rotation.x = .5f;
    _bounceSphere.rotation.y = sinf(t);
    _bounceSphere.rotation.z = .2f;
    _bounceSphere.rotation.w = t * 80;
    _bounceSphere.intensity = _syncBounceSphereIntensity;

    glTranslatef(0,0,-7);
    fx_dot_sphere_renderShadow(&_bounceSphere,1.15f,1,-1.35f);

    glEnable(GL_SCISSOR_TEST);
    glPushMatrix();
    {
        // Top part of sphere is not distorted.
        glScissor(0,FW_RENDER_HEIGHT-170,FW_RENDER_WIDTH,170);
        glPushMatrix();
        fx_dot_sphere_render(&_bounceSphere, 1, 1, 1);
        glPopMatrix();

        // Gradually distort bottom part of sphere.
        const int SCISSOR_HEIGHT = 4;
        for (int i=171; i<220; i+=SCISSOR_HEIGHT) {
            glScissor(0,FW_RENDER_HEIGHT-i,FW_RENDER_WIDTH,SCISSOR_HEIGHT);
            glPushMatrix();
            float s = (i-170) * .015f;
            fx_dot_sphere_render(&_bounceSphere, 1.f + s, 1.f - s*.67, 1);
            glPopMatrix();

        }
    }
    glPopMatrix();
    glDisable(GL_SCISSOR_TEST);
}

static void renderMountainsLogoFlash(void) {
    if (_syncMountainsLogoFlashAlpha > 0) {
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_NOTEQUAL, 0);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);


        fw_image_renderBegin(&getImages()[RES_IMG_MOUNTAINS_LOGO], 1);
        glTranslatef(60,_mountainsLogoPos.y,0);
        glColor4ub(255,255,255,_syncMountainsLogoFlashAlpha);
        fw_image_putImage(&getImages()[RES_IMG_MOUNTAINS_LOGO]);
        fw_image_renderEnd();
        fw_render_defaultBlendMode();

        glDisable(GL_ALPHA_TEST);
    }
}

static void renderBounceSpheresScene(fw_timer_data *time, fw_timer_data *globalTime) {
    // `globalTime` is needed because of music sync.

    if (_mountainsSunsetBgDims[2].w != 0) {
        fw_stencil_begin();
        {
            fw_stencil_activateStencilBuffer();

            fw_projection_ortho();
            for (int i=0; i<LEN(_mountainsSunsetBgDims); i++) {
                fw_quad_put(_mountainsSunsetBgDims[i].x, _mountainsSunsetBgDims[i].y, _mountainsSunsetBgDims[i].z, _mountainsSunsetBgDims[i].w);
            }

            fw_stencil_activateColorBuffer(0);
            renderSunsetBG();
        }
        fw_stencil_end();
    }

    renderBounceSpheresBayerQuad(_bounceSpheresBayerQuadPos.x, 46,34,47);
    renderMountainsLogo();

    if (_mountainsSunsetBgDims[2].w == 0 && time->elapsed > _bounceSpheresDelay) {
        renderMountainsLogoFlash();
        fw_projection_perspective();
        updateBounceSphereSync(time);
        updateBounceSphere(time);
        renderBounceSphere(globalTime);
    }

    renderBounceSpheresBayerQuad(_bounceSpheresBayerQuad2Pos.x, 0,0,0);

    const float t = time->elapsed;
    const float quadTransitionStartTime = 22+6.5;
    if (t > quadTransitionStartTime) {
        renderBounceSpheresQuadTransition((fw_vec3f) {
            1,1,1
        },quadTransitionStartTime,t);
    }
    if (t > quadTransitionStartTime+1) {
        renderBounceSpheresQuadTransition((fw_vec3f) {
            0,0,0
        },quadTransitionStartTime+1,t);
    }
    if (t > quadTransitionStartTime+2) {
        renderBounceSpheresQuadTransition((fw_vec3f) {
            1,1,1
        },quadTransitionStartTime+2,t);
    }
}

// +++++++++++ //
// SCENE: LOVE
// +++++++++++ //

static void renderIsoWobbler(fw_timer_data *time) {
    fw_projection_ortho();
    glColor3ub(64,49,141); // C64 blue.
    fw_quad_put(0,0, FW_RENDER_WIDTH, FW_RENDER_HEIGHT);

    glEnable(GL_SCISSOR_TEST);

    const float alpha = 50 + 4.f*sinf(64*time->elapsed) + 6.f*sinf(1.f+16*time->elapsed);
    const int scissorHeight=4;
    for (int i=0; i<FW_RENDER_HEIGHT; i+=scissorHeight) {
        glScissor(0,i,FW_RENDER_WIDTH,scissorHeight);
        const float scaleY = 4.25f*sinf(i*.15f + 4*time->elapsed);
        fw_image_renderBegin(&getImages()[RES_IMG_TITLE_BG], 1);
        glColor4ub(255,255,255,alpha);
        glTranslatef(0,scaleY,0);
        fw_image_putImage(&getImages()[RES_IMG_TITLE_BG]);
        fw_image_renderEnd();
    }

    glDisable(GL_SCISSOR_TEST);

    color_black();
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(0, 1);
    glVertex2f(FW_RENDER_WIDTH, 1);

    glVertex2f(0, FW_RENDER_HEIGHT-2);
    glVertex2f(FW_RENDER_WIDTH, FW_RENDER_HEIGHT-2);
    glEnd();
}

static void renderLoveFlash(fw_timer_data *time) {
    fw_projection_ortho();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(_syncLoveFlash,_syncLoveFlash,_syncLoveFlash,.3f);
    fw_quad_put(0,0,FW_RENDER_WIDTH,FW_RENDER_HEIGHT);
    fw_render_defaultBlendMode();
    _syncLoveFlash -= time->delta*2;
    if (_syncLoveFlash < 0) {
        _syncLoveFlash = 0;
    }
}

static void renderLoveScene(fw_timer_data *time) {
    const float t = time->elapsed;

    if (_loveTransitionEnabled.x == TRUE) {
        renderIsoWobbler(time);

        fw_projection_perspective();
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glTranslatef(_loveTransitionQuadPos.x, _loveTransitionQuadPos.y, _loveTransitionQuadPos.z);
        glRotatef(_loveTransitionQuadRot.w, _loveTransitionQuadRot.x, _loveTransitionQuadRot.y, _loveTransitionQuadRot.z);

        glPushMatrix();
        {
            color_white();
            glScalef(-1,1,1);
            fw_quad_put(-1,-1,2,2);
        }
        glPopMatrix();

        if (_loveTransitionStencilEnabled.x == TRUE) {
            fw_stencil_begin();
            {
                fw_stencil_activateStencilBuffer();

                glPushMatrix();
                fw_quad_put(-1,-1,2,2);
                glPopMatrix();
                glDisable(GL_CULL_FACE);

                fw_stencil_activateColorBuffer(0);

                fx_love_renderBG(_loveRainbowPos.x, t);
                renderLoveFlash(time);
            }
            fw_stencil_end();
        } else {
            glColor3ub(191,206,114); // C64 yellow.
            glPushMatrix();
            fw_quad_put(-1,-1,2,2);
            glPopMatrix();
            glDisable(GL_CULL_FACE);
        }

        glFrontFace(GL_CCW);
    } else {
        fx_love_renderBG(_loveRainbowPos.x, t);
        fx_hextiles_render(_hexTilesProgress.x);
        renderLoveFlash(time);
        fx_love_renderTexts(_loveTextPos.x, t);

    }
}

// +++++++++++++ //
// SCENE: GREETS
// +++++++++++++ //

static void renderGreetsBGScene(void) {
    glClearColor(70.f/255,70.f/255,70.f/255,1);
    glClear(GL_COLOR_BUFFER_BIT);
    fx_hextiles_render(_hexTilesProgress.x);

    glPushMatrix();
    color_black();
    fw_quad_put(_greetsTransitQuadDim.x, 0, FW_RENDER_WIDTH, FW_RENDER_HEIGHT);
    glPopMatrix();
}

static void updateGreetsSync(fw_timer_data *time) {
    _syncGreetsEvenPosX += _syncGreetsEvenSpeedX * time->delta;
    _syncGreetsOddPosX += _syncGreetsOddSpeedX * time->delta;

    _syncGreetsEvenSpeedX = fw_math_lerp(_syncGreetsEvenSpeedX, 0, _syncGreetsDecayX*time->delta);
    _syncGreetsOddSpeedX = fw_math_lerp(_syncGreetsOddSpeedX, 0, _syncGreetsDecayX*time->delta);
}

static void renderGreetsScene(fw_timer_data *time) {
    updateGreetsSync(time);
    fx_greetings_pseudo_3d_update(_syncGreetsEvenPosX, _syncGreetsOddPosX);
    fx_greetings_pseudo_3d_renderText(_greetsStencil1Pos.x, _greetsStencil2Pos.x, time);
}

// ++++++++++++++++++ //
// SCENE: ISO PATTERN
// ++++++++++++++++++ //

static void renderIsoPatternGradientBG(void) {
    unsigned char a = _isoPatternGradientAlpha.x;
    if (a <= 0) {
        return;
    }

    fw_projection_ortho();
    glColor4ub(24,36,66,a);
    fw_quad_put(0,42,FW_RENDER_WIDTH,38);
    fw_quad_put(0,192,FW_RENDER_WIDTH,38);

    glColor4ub(23,32,56,a);
    fw_quad_put(0,43,FW_RENDER_WIDTH,1);
    fw_quad_put(0,192+38-2,FW_RENDER_WIDTH,1);

    glColor4ub(25,40,76,a);
    fw_quad_put(0,80,FW_RENDER_WIDTH,38);
    fw_quad_put(0,154,FW_RENDER_WIDTH,38);
    fw_quad_put(0,78,FW_RENDER_WIDTH,1);
    fw_quad_put(0,154+38+1,FW_RENDER_WIDTH,1);

    glColor4ub(26,42,81,a);
    fw_quad_put(0,118,FW_RENDER_WIDTH,36);
    fw_quad_put(0,116,FW_RENDER_WIDTH,1);
    fw_quad_put(0,113,FW_RENDER_WIDTH,1);
    fw_quad_put(0,109,FW_RENDER_WIDTH,1);
    fw_quad_put(0,118+36+1,FW_RENDER_WIDTH,1);
    fw_quad_put(0,118+36+4,FW_RENDER_WIDTH,1);
    fw_quad_put(0,118+36+8,FW_RENDER_WIDTH,1);
}

static void renderIsoLogoShadow(void) {
    // Shadow below logo.
    fw_image_renderBegin(&getImages()[RES_IMG_CIRCLE_FILLED],1);
    glColor4ub(0,0,0,100);
    glTranslatef(_isoLogoShadowPos.x,_isoLogoShadowPos.y,0);
    glScalef(.25,.1,1);
    fw_image_putImage(&getImages()[RES_IMG_CIRCLE_FILLED]);
    fw_image_renderEnd();
}

static void renderIsoPatternBGScene(void) {
    glClearColor(_isoPatternBGColor.x,_isoPatternBGColor.y,_isoPatternBGColor.z,1);
    glClear(GL_COLOR_BUFFER_BIT);
    renderIsoLogoShadow();
}

static void updateIsoPatternFlashSync(fw_timer_data *time) {
    _syncIsoPatternFlashAlpha = fw_math_lerp(_syncIsoPatternFlashAlpha, 0, time->delta);
    _syncIsoLogoFlashAlpha = fw_math_lerp(_syncIsoLogoFlashAlpha, 0, time->delta*3);
}

static void renderIsoPatternFlash(void) {
    if (_syncIsoPatternFlashAlpha <= 0) {
        return;
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4ub(255,255,255,_syncIsoPatternFlashAlpha);
    fw_quad_put(0,0,FW_RENDER_WIDTH,FW_RENDER_HEIGHT);
    fw_render_defaultBlendMode();
}

static void renderIsoPatternScene(fw_timer_data *time) {
    renderIsoPatternGradientBG();
    updateIsoPatternFlashSync(time);

    _isoLogoMeta.x = _isoLogoPos.x + _isoLogoPos2.x;
    _isoLogoMeta.amp = _isoLogoAmp.x;
    _isoLogoMeta.patternVerticalSpread = _isoLogoVerticalSpread.x;
    _isoLogoMeta.p = _isoLogoPercentage.x;
    _isoLogoMeta.src = _isoLogoSrcDst.x;
    _isoLogoMeta.dst = _isoLogoSrcDst.y;
    _isoLogoMeta.outlineAlpha = _isoLogoOutlineAlpha.x;

    fx_isologo_renderPatternUpdate(&_isoLogoMeta, time);

    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();

        // Write to stencil buffer as well as to color buffer.
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // Also render transparent fragments (logo outline fades in).
        glAlphaFunc(GL_GREATER, 0);

        fx_isologo_renderBegin();
        fx_isologo_renderPattern(&_isoLogoMeta);
        fx_isologo_renderEnd();

        fw_stencil_activateColorBuffer(0);

        if (_syncIsoLogoFlashAlpha > 0) {
            glColor4ub(255,255,255, _syncIsoLogoFlashAlpha);
            fw_quad_put(10,0,40,FW_RENDER_HEIGHT);
        }
    }
    fw_stencil_end();

    renderIsoPatternFlash();
}

// ++++++++++++++++++++++++++ //
// SCENE: ISO LOGO TRANSITION
// ++++++++++++++++++++++++++ //

static void renderIsoLogoAndShadow(void) {
    renderIsoLogoShadow();

    // Logo.
    fx_isologo_renderBegin();
    glTranslatef(20,50,0);
    fx_isologo_render(FALSE, 0, FX_ISO_LOGO_FINAL_Y, FX_ISO_LOGO_FINAL_Y,255,255,255,255);
    fx_isologo_renderEnd();
}

static void renderIsoLogoTransitionScene(void) {
    glClearColor(23/255.f,32/255.f,56/255.f,1);
    glClear(GL_COLOR_BUFFER_BIT);

    fw_projection_ortho();

    renderIsoPatternGradientBG();

    const int maxPalIdx = _dotTunnelText3dPaletteSize-1;
    const int h = .5f*FW_RENDER_HEIGHT;

    for (int i=0; i<_numIsoLogoTransitionLinePositions; i++) {
        int palIdx = maxPalIdx * fabs(h - _isoLogoTransitionLinePositions[i].y) / h;
        palIdx = maxPalIdx - palIdx;
        glColor3ub(_dotTunnelText3dPalette[palIdx].x,_dotTunnelText3dPalette[palIdx].y,_dotTunnelText3dPalette[palIdx].z);
        fw_quad_put(i*2, _isoLogoTransitionLinePositions[i].y, 2,2);
    }

    renderIsoLogoAndShadow();
}

// +++++++++++++++++ //
// SCENE: DOT TUNNEL
// +++++++++++++++++ //

static void renderDotTunnelText3dScene(fw_timer_data *time) {
    text_3d_anim_params *p;
    fx_text_3d_params *ft;

    for (int i=0; i<NUM_DOT_TUNNEL_TEXT_PARAMS; i++) {
        p = &_dotTunnelText3dAnimParams[i];
        ft = &_dotTunnelTextParams[i];
        const fw_vec4f *ip = &_dotTunnelText3dPosInits[i];
        ft->pos.x = p->pos.x + p->sineAmp.x * sinf(p->sineOffset.x + time->elapsed * p->sineSpeed.x);
        ft->pos.y = p->pos.y + p->sineAmp.y * sinf(p->sineOffset.y + time->elapsed * p->sineSpeed.y);
        ft->pos.z = p->pos.z + p->sineAmp.z * sinf(p->sineOffset.z + time->elapsed * p->sineSpeed.z);
        ft->paletteIndex = p->palIndex.x;
        if (p->pos.z > ip->z && p->pos.z < 0) {
            fx_text_3d_render(ft);
        }
    }
}

static void renderDotTunnelSceneSync(fw_timer_data *time) {
    _syncDotTunnelBGFlashAlpha = fw_math_lerp(_syncDotTunnelBGFlashAlpha, 0, .5f*time->delta);
}

static void renderDotTunnelGradient(float width, float xoff, fw_timer_data *time) {
    fx_overlay_renderOverlay(RES_IMG_GRADIENT_TUNNEL, 255,255,255,255, xoff, 30*sinf(time->elapsed), width,1);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4ub(255,128,170,_syncDotTunnelBGFlashAlpha);
    fw_quad_put(xoff,0,width,FW_RENDER_HEIGHT);
    fw_render_defaultBlendMode();
}

static void renderCircle(float scale) {
    fw_image_renderBegin(&getImages()[RES_IMG_CIRCLE_FILLED], 1);
    scale *= 0.01;
    fw_projection_perspective();
    glTranslatef(0,0,-2);
    glScalef(scale,scale,scale);
    glTranslatef(-.5f*128,-.5*128,0);
    color_white();
    fw_image_putImage(&getImages()[RES_IMG_CIRCLE_FILLED]);
    fw_image_renderEnd();
}

static void renderDotTunnelOverlay(fw_timer_data *time) {
    fw_projection_ortho();

    // Left overlay.
    renderDotTunnelGradient(39,0, time);

    // Bottom Part
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();
        // Write both stencil and color buffer.
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glColor3ub(23,32,56);
        fw_quad_put(0,FW_RENDER_HEIGHT-_dotTunnelBorderBottomPos.x,FW_RENDER_WIDTH,_dotTunnelBorderBottomPos.x); // Bottom overlay.
        fw_quad_put(0,FW_RENDER_HEIGHT-_dotTunnelBorderBottomPos.x-1,FW_RENDER_WIDTH,1); // Bottom shadow line.

        fw_stencil_activateColorBuffer(0);
        renderIsoPatternGradientBG();

    }
    fw_stencil_end();

    glColor3ub(_dotTunnelText3dPalette[8].x,_dotTunnelText3dPalette[8].y,_dotTunnelText3dPalette[8].z);
    fw_quad_put(0,FW_RENDER_HEIGHT-_dotTunnelBorderBottomPos.x,FW_RENDER_WIDTH,2); // Bottom white line.

    if (_dotTunnelIsBorderTopFG.x) {
        renderIsoLogoAndShadow();
    }

    // Top Part
    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();
        // Write both stencil and color buffer.
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glColor3ub(23,32,56);
        fw_quad_put(0,0,FW_RENDER_WIDTH,_dotTunnelBorderTopPos.x); // Top overlay.
        fw_quad_put(0,_dotTunnelBorderTopPos.x+2,FW_RENDER_WIDTH,1); // Top shadow line.

        fw_stencil_activateColorBuffer(0);
        renderIsoPatternGradientBG();

    }
    fw_stencil_end();

    glColor3ub(_dotTunnelText3dPalette[8].x,_dotTunnelText3dPalette[8].y,_dotTunnelText3dPalette[8].z);
    fw_quad_put(0,_dotTunnelBorderTopPos.x,FW_RENDER_WIDTH,2); // Top white line.

    if (!_dotTunnelIsBorderTopFG.x) {
        renderIsoLogoAndShadow();
    }
}

static void renderDotTunnelScene(fw_timer_data *time) {
    renderDotTunnelSceneSync(time);

    renderDotTunnelGradient(FW_RENDER_WIDTH,39, time);

    fx_dot_tunnel_update(&_dotTunnel, 4.f, 20.f, &_dotTunnelAmp, time);

    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();
        renderCircle(_dotTunnelOverlayScale.x);

        fw_projection_perspective();

        // Draw default colors on part with stencil.
        fw_stencil_activateColorBuffer(1);
        _dotTunnel.dotColors = _dotTunnelDefaultColors;
        fx_dot_tunnel_render(&_dotTunnel, time);

        // Draw rainbow colors on part without stencil.
        fw_stencil_activateColorBuffer(0);
        _dotTunnel.dotColors = _dotTunnelRainbowColors;
        fx_dot_tunnel_render(&_dotTunnel, time);
    }
    fw_stencil_end();

    renderDotTunnelOverlay(time);
}


// ++++++++++++++++++++ //
// SCENE: CYLINDER TEXT
// ++++++++++++++++++++ //

static void text(const char string[], float x, float y, float s, int palIdx) {
    if (palIdx < 0) {
        return;
    }

    glPushMatrix();
    fw_font_renderTextBegin(getFontFace());

    glTranslatef(x, y, 0);
    glScalef(s,s,1);

    color_black();
    fw_font_renderSingleTextLine(string, getFontFace(), 2,2);

    glColor4ub(_presentsPalette[palIdx].x, _presentsPalette[palIdx].y, _presentsPalette[palIdx].z, _presentsPalette[palIdx].w);
    fw_font_renderSingleTextLine(string, getFontFace(), 0,0);
    fw_font_renderTextEnd();
    glPopMatrix();
}

static void renderCylinderTwistTextScene(void) {
    for (int i=0; i<NUM_CYLINDER_TEXT_LINES; i++) {
        text("ASSEMBLE THE TRACKS...", 30,5 + i*20, 1, _cylinderText1PaletteIndexes[i].x);
        text("AND START THE RACE!", 45,5 + i*20, 1, _cylinderText2PaletteIndexes[i].x);
    }
}

// ++++++++++++++++++++++++++ //
// SCENE: CYLINDER TRANSITION
// ++++++++++++++++++++++++++ //

static void renderCylinderTwistTransitionScene(fw_timer_data *time) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, getImages()[RES_IMG_CHECKER].textureID);

    fw_projection_perspective();

    glTranslatef(0,0, _cylinderCheckerZoom.x);

    for (int i=0; i<LEN(_cylinderCheckerRowPositions); i++) {
        glColor4ub(_cylinderCheckerRowColors[i].x,
                   _cylinderCheckerRowColors[i].y,
                   _cylinderCheckerRowColors[i].z,
                   _cylinderCheckerRowColors[i].w);

        for (int j=0; j<2; j++) {
            glPushMatrix();
            {
                glTranslatef(_cylinderCheckerRowPositions[i].x + j*.25f,_cylinderCheckerRowPositions[i].y,-1.5);
                glRotatef(_cylinderCheckerRowRotations[i].w,
                          _cylinderCheckerRowRotations[i].x,
                          _cylinderCheckerRowRotations[i].y,
                          _cylinderCheckerRowRotations[i].z);
                glScalef(1/16.f,1/16.f,1);
                fw_quad_putTextured(.5,0, .5,0);
            }
            glPopMatrix();
        }
    }

    glDisable(GL_TEXTURE_2D);

    fx_overlay_renderOverlay(RES_IMG_GRADIENT_GROUND,
                             _cylinderOverlayColor.x,
                             _cylinderOverlayColor.y,
                             _cylinderOverlayColor.z,
                             _cylinderOverlayColor.w,
                             0,0, 3,3);
}


// +++++++++++++++++++++ //
// SCENE: CYLINDER TWIST
// +++++++++++++++++++++ //

static float _cylinderTwistCheckerTextureOffsetY = 0;
static fw_vec4f _cylinderCheckerMiddleLayerColor = {122,54,123,255};

static void updateCylinderTwistBGScene(fw_timer_data *time) {
    _cylinderTwistCheckerTextureOffsetY += time->delta * _cylinderCheckerSpeed.y;
}

static void updateCylinderTwistBGSceneSync(fw_timer_data *time) {
    const float dt = 2*time->delta;

    _syncCylinderCheckerGroundLayerColor.x = fw_math_lerp(_syncCylinderCheckerGroundLayerColor.x, 0, dt);
    _syncCylinderCheckerGroundLayerColor.y = fw_math_lerp(_syncCylinderCheckerGroundLayerColor.y, 0, dt);
    _syncCylinderCheckerGroundLayerColor.z = fw_math_lerp(_syncCylinderCheckerGroundLayerColor.z, 0, dt);

    _syncCylinderCheckerMiddleLayerColor.x = fw_math_lerp(_syncCylinderCheckerMiddleLayerColor.x, 0, dt);
    _syncCylinderCheckerMiddleLayerColor.y = fw_math_lerp(_syncCylinderCheckerMiddleLayerColor.y, 0, dt);
    _syncCylinderCheckerMiddleLayerColor.z = fw_math_lerp(_syncCylinderCheckerMiddleLayerColor.z, 0, dt);

    _syncCylinderCheckerTopLayerColor.x = fw_math_lerp(_syncCylinderCheckerTopLayerColor.x, 0, dt);
    _syncCylinderCheckerTopLayerColor.y = fw_math_lerp(_syncCylinderCheckerTopLayerColor.y, 0, dt);
    _syncCylinderCheckerTopLayerColor.z = fw_math_lerp(_syncCylinderCheckerTopLayerColor.z, 0, dt);
}

static void renderCylinderTwistBGScene(fw_timer_data *time) {
    updateCylinderTwistBGScene(time);
    updateCylinderTwistBGSceneSync(time);

    fw_projection_perspective();

    glPushMatrix();
    {
        glTranslatef(0,0, _cylinderCheckerZoom.x);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, getImages()[RES_IMG_CHECKER].textureID);

        // Ground layer.
        glPushMatrix();
        {
            glColor4ub(_cylinderCheckerGroundLayerColor.x + _syncCylinderCheckerGroundLayerColor.x,
                       _cylinderCheckerGroundLayerColor.y + _syncCylinderCheckerGroundLayerColor.y,
                       _cylinderCheckerGroundLayerColor.z + _syncCylinderCheckerGroundLayerColor.z,
                       _cylinderCheckerGroundLayerColor.w);
            glTranslatef(0,0,-1.5);
            glScalef(1,2,1);
            fw_quad_putTextured(8,0, 16,_cylinderTwistCheckerTextureOffsetY);
        }
        glPopMatrix();

        // Middle layer.
        glColor3ub(_cylinderCheckerMiddleLayerColor.x + _syncCylinderCheckerMiddleLayerColor.x,
                   _cylinderCheckerMiddleLayerColor.y + _syncCylinderCheckerMiddleLayerColor.y,
                   _cylinderCheckerMiddleLayerColor.z + _syncCylinderCheckerMiddleLayerColor.z);
        glPushMatrix();
        {
            glTranslatef(-_cylinderCheckerMiddelSpread.x,0,0);
            fw_quad_putTextured(8,0, 8,_cylinderTwistCheckerTextureOffsetY);
        }
        glPopMatrix();
        glPushMatrix();
        {
            glTranslatef(_cylinderCheckerMiddelSpread.x,0,0);
            fw_quad_putTextured(8,0, 8,_cylinderTwistCheckerTextureOffsetY);
        }
        glPopMatrix();

        // Shadows of top layer.
        glColor4ub(0,0,0,127);
        glPushMatrix();
        {
            glTranslatef(-1.35,-0.05,.5);
            fw_quad_putTextured(7,0, 7,_cylinderTwistCheckerTextureOffsetY);
        }
        glPopMatrix();
        glPushMatrix();
        {
            glTranslatef(1.45,-0.05,.5);
            fw_quad_putTextured(7,0, 7,_cylinderTwistCheckerTextureOffsetY);
        }
        glPopMatrix();

        // Top layer.
        glColor3ub( 26 + _syncCylinderCheckerTopLayerColor.x,
                    90 + _syncCylinderCheckerTopLayerColor.y,
                    150 + _syncCylinderCheckerTopLayerColor.z);
        glPushMatrix();
        {
            glTranslatef(-1.4,0,.5);
            fw_quad_putTextured(7,0, 7,_cylinderTwistCheckerTextureOffsetY);
        }
        glPopMatrix();
        glPushMatrix();
        {
            glTranslatef(1.4,0,.5);
            fw_quad_putTextured(7,0, 7,_cylinderTwistCheckerTextureOffsetY);
        }
        glPopMatrix();

    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Gradient.
    fx_overlay_renderOverlay(RES_IMG_GRADIENT_GROUND,
                             _cylinderOverlayColor.x,
                             _cylinderOverlayColor.y,
                             _cylinderOverlayColor.z,
                             _cylinderOverlayColor.w,
                             0,0, 3,3);

    // Transition out overlay.
    if (_cylinderTwistTransitionOutQuadHeight.x != 0) {
        glColor3ub(_cylinderCheckerMiddleLayerColor.x,
                   _cylinderCheckerMiddleLayerColor.y,
                   _cylinderCheckerMiddleLayerColor.z);
        fw_quad_put(0,FW_RENDER_HEIGHT,FW_RENDER_WIDTH, _cylinderTwistTransitionOutQuadHeight.x);
    }
}

static void updateCylinderTwistSceneSync(fw_timer_data *time) {
    const float dt = 4*time->delta;
    _syncCylinderColorAdd.x = fw_math_lerp(_syncCylinderColorAdd.x, 0, dt);
    _syncCylinderColorAdd.y = fw_math_lerp(_syncCylinderColorAdd.y, 0, dt);
    _syncCylinderColorAdd.z = fw_math_lerp(_syncCylinderColorAdd.z, 0, dt);
}

static void renderCylinderTwistScene(fw_timer_data *time) {
    updateCylinderTwistSceneSync(time);
    fw_projection_perspective();

    _cylinderParams.twistVariance = _cylinderTwistVariance.x;

    _cylinderParams.cylinders[0].slices = _cylinderTwistSlices.x;
    _cylinderParams.cylinders[0].baseRadius = _cylinderTwistBaseRadius.x;
    _cylinderParams.cylinders[0].topRadius = _cylinderTwistTopRadius.x;

    _cylinderParams.cylinders[0].color.x = 225 + _syncCylinderColorAdd.x;
    _cylinderParams.cylinders[0].color.y = 50 + _syncCylinderColorAdd.y;
    _cylinderParams.cylinders[0].color.z = 66  + _syncCylinderColorAdd.z;

    _cylinderParams.cylinders[1].slices = _cylinderTwistSlices.y;
    _cylinderParams.cylinders[1].baseRadius = _cylinderTwistBaseRadius.y;
    _cylinderParams.cylinders[1].topRadius = _cylinderTwistTopRadius.y;

    _cylinderParams.cylinders[2].slices = _cylinderTwistSlices.z;
    _cylinderParams.cylinders[2].baseRadius = _cylinderTwistBaseRadius.z;
    _cylinderParams.cylinders[2].topRadius = _cylinderTwistTopRadius.z;

    _cylinderParams.distance = _cylinderTwistDistance.x + _cylinderTwistDistance.y * sinf(time->elapsed * .5f);

    _cylinderParams.cylinders[0].yDelta = -2.f  + _cylinderTwistYSineAmp.x * sinf(time->elapsed)          + _cylinderYDeltas.x;
    _cylinderParams.cylinders[1].yDelta = -2.f  + _cylinderTwistYSineAmp.y * sinf(1.5f+time->elapsed*.5f) + _cylinderYDeltas.y;
    _cylinderParams.cylinders[2].yDelta = -2.5f + _cylinderTwistYSineAmp.z * sinf(2.5f+time->elapsed*0.9) + _cylinderYDeltas.z;

    fx_cylinder_twist_render(&_cylinderParams, time);
}

// ++++++++++++++++++++++++++++ //
// SCENE: END BLINDS TRANSITION
// ++++++++++++++++++++++++++++ //

static void renderEndBlinds(float delay, unsigned char r,unsigned char g,unsigned char b, fw_timer_data *time) {
    float t = time->elapsed - delay;

    fw_projection_ortho();
    glColor3ub(r,g,b);

    const int h = 27/2;
    const float h2 = h*.5f;
    const int numStrips = FW_RENDER_HEIGHT/h + 1;
    const int numWaves = 2; // Multiple of 2.
    const float speed = 7.f;

    for (int i=0; i<numStrips; i++) {
        const float freq = M_PI + t * speed - i*.4f;

        // Skip if first wave has not been passed.
        if (freq < M_PI ) {
            continue;
        }

        float sy;
        if (freq > numWaves*M_PI) {
            // Wave limit has been reached. Scale quad to full height.
            sy = 1;
        } else {
            // Else calculate scale factor depending on freq value.
            sy = .5f + 0.49f*cosf(freq);
        }

        glPushMatrix();
        glTranslatef(0,FW_RENDER_HEIGHT-h - i*h,0);
        glTranslatef(0,h2,0);
        glScalef(1,sy,1);
        glTranslatef(0,-h2,0);
        fw_quad_put(0,0,FW_RENDER_WIDTH,h);
        glPopMatrix();
    }
}

static void renderEndBlindsTransitionScene(fw_timer_data *time) {
    renderEndBlinds(0,  76,44,49, time);
    renderEndBlinds(.2, 106,54,51, time);
    renderEndBlinds(.4, 137,64,54, time);
    renderEndBlinds(.6, 144,95,37, time);
    renderEndBlinds(.8, 187,119,109, time);

    renderEndBlinds(1, 144,95,37, time);
    renderEndBlinds(1.2, 137,64,54, time);
    renderEndBlinds(1.4, 106,54,51, time);
    renderEndBlinds(1.6,  76,44,49, time);
}

// +++++++++++++++++++ //
// SCENE: END SCROLLER
// +++++++++++++++++++ //

static void updateEndScrollerMusicSync(fw_timer_data *time) {
    const float dt = 2*time->delta;

    for (int i=0; i<_numEndScrollerRowColors; i++) {
        _endScrollerRowColors[i].w = fw_math_lerp(_endScrollerRowColors[i].w, 0, dt);
    }
}

static void renderEndScrollerMusic(void) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPushMatrix();
    glTranslatef(145,8+7*32,0);
    for (int i=0; i<_numEndScrollerRowColors; i++) {
        if (i!=0
                && i!=2
                && i!=5
                && i!=6
                && i!=7
                && i!=9
                && i!=11
                && i!=12) {
            continue;
        }
        glColor4ub(_endScrollerRowColors[i].x,
                   _endScrollerRowColors[i].y,
                   _endScrollerRowColors[i].z,
                   _endScrollerRowColors[i].w);
        fw_quad_put(0,0,60,30);
        glTranslatef(0,-32,0);
    }
    glPopMatrix();
    fw_render_defaultBlendMode();
}

static void renderEndScrollerScene(fw_timer_data *time) {
    updateEndScrollerMusicSync(time);
    renderEndScrollerMusic();

    fw_stencil_begin();
    {
        fw_stencil_activateStencilBuffer();
        fx_overlay_renderBayerBorderTopBottom(0,0,255,255,255);
        fx_overlay_renderBayerBorderTopBottom(3,2,255,255,255);

        fw_stencil_activateColorBuffer(1);

        // Hack: Overwrite default line height for end part.
        getFontFace()->lineHeight = 28;

        const float speed= 16.7f;
        const float xOff = 96;
        const float yOff = 345;

        glPushMatrix();
        fw_font_renderTextBegin(getFontFace());
        glColor4ub(0,0,0, 120);
        glTranslatef(xOff + 4*sinf(time->elapsed*1), yOff-time->elapsed*speed + 3*sinf(1+time->elapsed*.7),0);
        glScalef(.5,.5,1);
        fw_font_renderText(_endScrollerText, _numLinesEndScrollerText, getFontFace(), 0,0, 1);
        fw_font_renderTextEnd();

        fw_font_renderTextBegin(getFontFace());
        glColor3ub(255,255,255);
        glTranslatef(xOff, yOff-time->elapsed*speed,0);
        glScalef(.5,.5,1);
        fw_font_renderText(_endScrollerText, _numLinesEndScrollerText, getFontFace(), 0,0, 1);
        fw_font_renderTextEnd();
        glPopMatrix();
    }
    fw_stencil_end();

    // Left/right lines.
    glColor3ub(0,0,0);
    fw_quad_put(_endScrollerLineDims[0].x,_endScrollerLineDims[0].y,_endScrollerLineDims[0].z,_endScrollerLineDims[0].w);
    fw_quad_put(_endScrollerLineDims[1].x,_endScrollerLineDims[1].y,_endScrollerLineDims[1].z,_endScrollerLineDims[1].w);

    if (_creditsBGHeight.x > 0) {
        glColor3ub(0,0,0);
        fw_quad_put(0,0,FW_RENDER_WIDTH,_creditsBGHeight.x);
    }
}

#if FW_SCANLINES_ENABLED
static void renderScanlines(void) {
    fw_render_viewportHiRes(0,0);
    fw_projection_ortho();
    glColor4f(0,0,0,FW_SCANLINES_ALPHA);
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_LINES);
    for (float i=0; i<FW_WINDOW_HEIGHT/2; i+=1) {
        glVertex2f(0, i);
        glVertex2f(FW_WINDOW_WIDTH, i);
    }
    glEnd();
    glPopMatrix();
}
#endif

void scene_0_execute(fw_timer_data *time) {
    fw_sync_process(&_syncData, time);
    fw_animation_processAnimationSchedule(&_animSchedule, time);

    fw_timer_data st;

    // Low-res.
    fw_render_bindTextureBufferBegin();
    {
        fw_render_viewportLowRes(0,0);

        if (isRunning(ANIM_TITLE_LOGO)) {
            st = sceneTime(ANIM_TITLE_LOGO, time);
            renderTitleScene(&st);
        }
        if (isRunning(ANIM_ENDLESS_GRID)) {
            st = sceneTime(ANIM_ENDLESS_GRID, time);
            renderEndlessGridScene(&st);
        }
        if (isRunning(ANIM_TWIRL_SPHERE)) {
            st = sceneTime(ANIM_TWIRL_SPHERE, time);
            renderTwirlSphereScene(&st);
        }
        if (isRunning(ANIM_MOUNTAINS_SCROLLER)) {
            st = sceneTime(ANIM_MOUNTAINS_SCROLLER, time);
            renderMountainsScene(&st);
        }
        if (isRunning(ANIM_BOUNCE_SPHERES)) {
            st = sceneTime(ANIM_BOUNCE_SPHERES, time);
            renderBounceSpheresScene(&st, time);
        }
        if (isRunning(ANIM_LOVE)) {
            st = sceneTime(ANIM_LOVE, time);
            renderLoveScene(&st);
        }
        if (isRunning(ANIM_GREETS)) {
            renderGreetsBGScene();
        }
        if (isRunning(ANIM_ISO_PATTERN) || isRunning(ANIM_ISO_PATTERN_IN) || isRunning(ANIM_ISO_PATTERN_OUT)) {
            renderIsoPatternBGScene();
        }
        if (isRunning(ANIM_ISO_LOGO_TRANSITION) || isRunning(ANIM_ISO_LOGO_TRANSITION_LINE)) {
            renderIsoLogoTransitionScene();
        }
        if (isRunning(ANIM_DOT_TUNNEL)) {
            st = sceneTime(ANIM_DOT_TUNNEL, time);
            renderDotTunnelScene(&st);
        }
        if (isRunning(ANIM_CYLINDER_TWIST)) {
            st = sceneTime(ANIM_CYLINDER_TWIST, time);
            renderCylinderTwistScene(&st);
        }
        if (isRunning(ANIM_END_BLINDS_TRANSITION)) {
            st = sceneTime(ANIM_END_BLINDS_TRANSITION, time);
            renderEndBlindsTransitionScene(&st);
        }
    }
    fw_render_bindTextureBufferEnd();

    // High-res.
    fw_render_viewportHiRes(0,0);
    {
        fw_render_clearHiRes(_globalClearColor.x,_globalClearColor.y,_globalClearColor.z,_globalClearColor.w);

        if (isRunning(ANIM_PRESENTS) || isRunning(ANIM_PRESENTS_TEXT1)) {
            st = sceneTime(ANIM_PRESENTS, time);
            renderPresentsScene(&st);
        }
        if (isRunning(ANIM_ENDLESS_GRID)) {
            renderEndlessGridBGScene();
        }
        if (isRunning(ANIM_BOUNCE_SPHERES)) {
            st = sceneTime(ANIM_BOUNCE_SPHERES, time);
            renderBounceSpheresBGScene(&st);
        }
        if (isRunning(ANIM_CYLINDER_TWIST_TRANSITION)) {
            st = sceneTime(ANIM_CYLINDER_TWIST_TRANSITION, time);
            renderCylinderTwistTransitionScene(&st);
        }
        if (isRunning(ANIM_CYLINDER_TWIST)) {
            st = sceneTime(ANIM_CYLINDER_TWIST, time);
            renderCylinderTwistBGScene(&st);
        }
        if (isRunning(ANIM_CYLINDER_TWIST_TEXT1) || isRunning(ANIM_CYLINDER_TWIST_TEXT2)) {
            renderCylinderTwistTextScene();
        }
        if (isRunning(ANIM_END_SCROLLER)) {
            st = sceneTime(ANIM_END_SCROLLER, time);
            renderEndScrollerScene(&st);
        }
    }

    // Render low-res content from texture.
    fw_render_renderTextureBuffer();

    // Render additional hi-res content.
    fw_render_viewportHiRes(0,0);
    if (isRunning(ANIM_MOUNTAINS_SCROLLER)) {
        renderMountainsTextScrollerScene();
    }
    if (isRunning(ANIM_GREETS)) {
        st = sceneTime(ANIM_GREETS, time);
        renderGreetsScene(&st);
    }
    if (isRunning(ANIM_ISO_PATTERN) || isRunning(ANIM_ISO_PATTERN_IN) || isRunning(ANIM_ISO_PATTERN_OUT)) {
        st = sceneTime(ANIM_ISO_PATTERN, time);
        renderIsoPatternScene(&st);
    }
    if (isRunning(ANIM_DOT_TUNNEL)) {
        st = sceneTime(ANIM_DOT_TUNNEL, time);
        renderDotTunnelText3dScene(&st);
    }

#if FW_SCANLINES_ENABLED
    renderScanlines();
#endif

}
