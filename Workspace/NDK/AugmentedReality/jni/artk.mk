LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := artk

IDIR := ../ARToolKit/include
DIR := ../ARToolKit/lib/SRC

SOURCES := 	$(DIR)/AR/arDetectMarker.c \
 $(DIR)/AR/arDetectMarker2.c \
 $(DIR)/AR/arGetCode.c \
 $(DIR)/AR/arGetMarkerInfo.c \
 $(DIR)/AR/arGetTransMat.c \
 $(DIR)/AR/arGetTransMat2.c \
 $(DIR)/AR/arGetTransMat3.c \
 $(DIR)/AR/arGetTransMatCont.c \
 $(DIR)/AR/arLabeling.c \
 $(DIR)/AR/arUtil.c \
 $(DIR)/AR/mAlloc.c \
 $(DIR)/AR/mAllocDup.c \
 $(DIR)/AR/mAllocInv.c \
 $(DIR)/AR/mAllocMul.c \
 $(DIR)/AR/mAllocTrans.c \
 $(DIR)/AR/mAllocUnit.c \
 $(DIR)/AR/mDet.c \
 $(DIR)/AR/mDisp.c \
 $(DIR)/AR/mDup.c \
 $(DIR)/AR/mFree.c \
 $(DIR)/AR/mInv.c \
 $(DIR)/AR/mMul.c \
 $(DIR)/AR/mPCA.c \
 $(DIR)/AR/mSelfInv.c \
 $(DIR)/AR/mTrans.c \
 $(DIR)/AR/mUnit.c \
 $(DIR)/AR/paramChangeSize.c \
 $(DIR)/AR/paramDecomp.c \
 $(DIR)/AR/paramDisp.c \
 $(DIR)/AR/paramDistortion.c \
 $(DIR)/AR/paramFile.c \
 $(DIR)/AR/paramGet.c \
 $(DIR)/AR/vAlloc.c \
 $(DIR)/AR/vDisp.c \
 $(DIR)/AR/vFree.c \
 $(DIR)/AR/vHouse.c \
 $(DIR)/AR/vInnerP.c \
 $(DIR)/AR/vTridiag.c \
 $(DIR)/ARMulti/arMultiActivate.c \
 $(DIR)/ARMulti/arMultiGetTransMat.c \
 $(DIR)/ARMulti/arMultiReadConfigFile.c \
 \
 $(DIR)/VideoLinuxV4L/ccvt_c.c \
 $(DIR)/VideoLinuxV4L/video.c \
 \
 $(DIR)/Gl/gsub.c \
 $(DIR)/Gl/gsubUtil.c \
# $(DIR)/Gl/gsub_lite.c \

		  
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
 $(LOCAL_PATH)/$(IDIR) \
 $(LOCAL_PATH)/../../libNanoGLHL/NGL \
 apps/libNanoGLHL/NGL \

# -DAR_DEFAULT_PIXEL_FORMAT=AR_PIXEL_FORMAT_RGB \

LOCAL_CFLAGS := -DLINUX  -DANDROID -I/cygdrive/c/Temp/DOCS/Workspace/libNanoGLHL/NGL

#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

LOCAL_SRC_FILES := $(SOURCES)

LOCAL_LDLIBS := -L$(LOCAL_PATH)/lib -Ljni/lib -llog -lGLESv1_CM

LOCAL_STATIC_LIBRARIES 	:= libNanoGL

include $(BUILD_SHARED_LIBRARY)

$(call import-module,libNanoGLHL)

#dump:
#	@echo inc=$(LOCAL_C_INCLUDES)