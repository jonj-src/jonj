/*
  +----------------------------------------------------------------------+
  | JONJ Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the JONJ license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.jonj.tk/license/3_01.txt                                  |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_COMPAT_H
#define JONJ_COMPAT_H

#ifdef JONJ_WIN32
#include "config.w32.h"
#else
#include <jonj_config.h>
#endif

#if defined(HAVE_BUNDLED_PCRE) || !defined(JONJ_VERSION)
#define pcre_compile			jonj_pcre_compile
#define pcre_compile2			jonj_pcre_compile2
#define pcre_copy_substring		jonj_pcre_copy_substring
#define pcre_exec			jonj_pcre_exec
#define pcre_get_substring		jonj_pcre_get_substring
#define pcre_get_substring_list		jonj_pcre_get_substring_list
#define pcre_maketables			jonj_pcre_maketables
#define pcre_study			jonj_pcre_study
#define pcre_version			jonj_pcre_version
#define pcre_fullinfo			jonj_pcre_fullinfo
#define pcre_free			jonj_pcre_free
#define pcre_malloc			jonj_pcre_malloc
#define pcre_config			jonj_pcre_config
#define pcre_copy_named_substring	jonj_pcre_copy_named_substring
#define pcre_free_substring		jonj_pcre_free_substring
#define pcre_free_substring_list	jonj_pcre_free_substring_list
#define pcre_get_named_substring	jonj_pcre_get_named_substring
#define pcre_get_stringnumber		jonj_pcre_get_stringnumber
#define pcre_refcount			jonj_pcre_refcount
#define _pcre_ord2utf8			jonj__pcre_ord2utf8
#define _pcre_try_flipped		jonj__pcre_try_flipped
#define _pcre_valid_utf8		jonj__pcre_valid_utf8
#define _pcre_xclass			jonj__pcre_xclass
#define pcre_callout			jonj_pcre_callout
#define _pcre_OP_lengths		jonj__pcre_OP_lengths
#define _pcre_utt_names			jonj__pcre_utt_names
#define _pcre_default_tables		jonj__pcre_default_tables
#define pcre_get_stringtable_entries	jonj_pcre_get_stringtable_entries
#define _pcre_is_newline		jonj__pcre_is_newline
#define pcre_stack_free			jonj_pcre_stack_free
#define pcre_stack_malloc		jonj_pcre_stack_malloc
#define _pcre_utf8_table1		jonj__pcre_utf8_table1
#define _pcre_utf8_table1_size		jonj__pcre_utf8_table1_size
#define _pcre_utf8_table2		jonj__pcre_utf8_table2
#define _pcre_utf8_table3		jonj__pcre_utf8_table3
#define _pcre_utf8_table4		jonj__pcre_utf8_table4
#define _pcre_utt			jonj__pcre_utt
#define _pcre_utt_size			jonj__pcre_utt_size
#define _pcre_was_newline		jonj__pcre_was_newline
#define _pcre_ucd_records		jonj__pcre_ucd_records
#define _pcre_ucd_stage1		jonj__pcre_ucd_stage1
#define _pcre_ucd_stage2		jonj__pcre_ucd_stage2
#define _pcre_ucp_gentype		jonj__pcre_ucp_gentype
#endif

#define lookup				jonj_lookup
#define hashTableInit		jonj_hashTableInit
#define hashTableDestroy	jonj_hashTableDestroy
#define hashTableIterInit	jonj_hashTableIterInit
#define hashTableIterNext	jonj_hashTableIterNext

#if defined(HAVE_LIBXML) && (defined(HAVE_XML) || defined(HAVE_XMLRPC)) && !defined(HAVE_LIBEXPAT)
#define XML_DefaultCurrent jonj_XML_DefaultCurrent
#define XML_ErrorString jonj_XML_ErrorString
#define XML_ExpatVersion jonj_XML_ExpatVersion
#define XML_ExpatVersionInfo jonj_XML_ExpatVersionInfo
#define XML_ExternalEntityParserCreate jonj_XML_ExternalEntityParserCreate
#define XML_GetBase jonj_XML_GetBase
#define XML_GetBuffer jonj_XML_GetBuffer
#define XML_GetCurrentByteCount jonj_XML_GetCurrentByteCount
#define XML_GetCurrentByteIndex jonj_XML_GetCurrentByteIndex
#define XML_GetCurrentColumnNumber jonj_XML_GetCurrentColumnNumber
#define XML_GetCurrentLineNumber jonj_XML_GetCurrentLineNumber
#define XML_GetErrorCode jonj_XML_GetErrorCode
#define XML_GetIdAttributeIndex jonj_XML_GetIdAttributeIndex
#define XML_GetInputContext jonj_XML_GetInputContext
#define XML_GetSpecifiedAttributeCount jonj_XML_GetSpecifiedAttributeCount
#define XmlGetUtf16InternalEncodingNS jonj_XmlGetUtf16InternalEncodingNS
#define XmlGetUtf16InternalEncoding jonj_XmlGetUtf16InternalEncoding
#define XmlGetUtf8InternalEncodingNS jonj_XmlGetUtf8InternalEncodingNS
#define XmlGetUtf8InternalEncoding jonj_XmlGetUtf8InternalEncoding
#define XmlInitEncoding jonj_XmlInitEncoding
#define XmlInitEncodingNS jonj_XmlInitEncodingNS
#define XmlInitUnknownEncoding jonj_XmlInitUnknownEncoding
#define XmlInitUnknownEncodingNS jonj_XmlInitUnknownEncodingNS
#define XML_ParseBuffer jonj_XML_ParseBuffer
#define XML_Parse jonj_XML_Parse
#define XML_ParserCreate_MM jonj_XML_ParserCreate_MM
#define XML_ParserCreateNS jonj_XML_ParserCreateNS
#define XML_ParserCreate jonj_XML_ParserCreate
#define XML_ParserFree jonj_XML_ParserFree
#define XmlParseXmlDecl jonj_XmlParseXmlDecl
#define XmlParseXmlDeclNS jonj_XmlParseXmlDeclNS
#define XmlPrologStateInitExternalEntity jonj_XmlPrologStateInitExternalEntity
#define XmlPrologStateInit jonj_XmlPrologStateInit
#define XML_SetAttlistDeclHandler jonj_XML_SetAttlistDeclHandler
#define XML_SetBase jonj_XML_SetBase
#define XML_SetCdataSectionHandler jonj_XML_SetCdataSectionHandler
#define XML_SetCharacterDataHandler jonj_XML_SetCharacterDataHandler
#define XML_SetCommentHandler jonj_XML_SetCommentHandler
#define XML_SetDefaultHandlerExpand jonj_XML_SetDefaultHandlerExpand
#define XML_SetDefaultHandler jonj_XML_SetDefaultHandler
#define XML_SetDoctypeDeclHandler jonj_XML_SetDoctypeDeclHandler
#define XML_SetElementDeclHandler jonj_XML_SetElementDeclHandler
#define XML_SetElementHandler jonj_XML_SetElementHandler
#define XML_SetEncoding jonj_XML_SetEncoding
#define XML_SetEndCdataSectionHandler jonj_XML_SetEndCdataSectionHandler
#define XML_SetEndDoctypeDeclHandler jonj_XML_SetEndDoctypeDeclHandler
#define XML_SetEndElementHandler jonj_XML_SetEndElementHandler
#define XML_SetEndNamespaceDeclHandler jonj_XML_SetEndNamespaceDeclHandler
#define XML_SetEntityDeclHandler jonj_XML_SetEntityDeclHandler
#define XML_SetExternalEntityRefHandlerArg jonj_XML_SetExternalEntityRefHandlerArg
#define XML_SetExternalEntityRefHandler jonj_XML_SetExternalEntityRefHandler
#define XML_SetNamespaceDeclHandler jonj_XML_SetNamespaceDeclHandler
#define XML_SetNotationDeclHandler jonj_XML_SetNotationDeclHandler
#define XML_SetNotStandaloneHandler jonj_XML_SetNotStandaloneHandler
#define XML_SetParamEntityParsing jonj_XML_SetParamEntityParsing
#define XML_SetProcessingInstructionHandler jonj_XML_SetProcessingInstructionHandler
#define XML_SetReturnNSTriplet jonj_XML_SetReturnNSTriplet
#define XML_SetStartCdataSectionHandler jonj_XML_SetStartCdataSectionHandler
#define XML_SetStartDoctypeDeclHandler jonj_XML_SetStartDoctypeDeclHandler
#define XML_SetStartElementHandler jonj_XML_SetStartElementHandler
#define XML_SetStartNamespaceDeclHandler jonj_XML_SetStartNamespaceDeclHandler
#define XML_SetUnknownEncodingHandler jonj_XML_SetUnknownEncodingHandler
#define XML_SetUnparsedEntityDeclHandler jonj_XML_SetUnparsedEntityDeclHandler
#define XML_SetUserData jonj_XML_SetUserData
#define XML_SetXmlDeclHandler jonj_XML_SetXmlDeclHandler
#define XmlSizeOfUnknownEncoding jonj_XmlSizeOfUnknownEncoding
#define XML_UseParserAsHandlerArg jonj_XML_UseParserAsHandlerArg
#define XmlUtf16Encode jonj_XmlUtf16Encode
#define XmlUtf8Encode jonj_XmlUtf8Encode
#define XML_FreeContentModel jonj_XML_FreeContentModel
#define XML_MemMalloc jonj_XML_MemMalloc
#define XML_MemRealloc jonj_XML_MemRealloc
#define XML_MemFree jonj_XML_MemFree
#define XML_UseForeignDTD jonj_XML_UseForeignDTD
#define XML_GetFeatureList jonj_XML_GetFeatureList
#define XML_ParserReset jonj_XML_ParserReset

#ifdef HAVE_GD_BUNDLED
#define any2eucjp jonj_gd_any2eucjp
#define createwbmp jonj_gd_createwbmp
#define empty_output_buffer jonj_gd_empty_output_buffer
#define fill_input_buffer jonj_gd_fill_input_buffer
#define freewbmp jonj_gd_freewbmp
#define gdAlphaBlend jonj_gd_gdAlphaBlend
#define gdCompareInt jonj_gd_gdCompareInt
#define gdCosT jonj_gd_gdCosT
#define gdCtxPrintf jonj_gd_gdCtxPrintf
#define gdDPExtractData jonj_gd_gdDPExtractData
#define gdFontGetGiant jonj_gd_gdFontGetGiant
#define gdFontGetLarge jonj_gd_gdFontGetLarge
#define gdFontGetMediumBold jonj_gd_gdFontGetMediumBold
#define gdFontGetSmall jonj_gd_gdFontGetSmall
#define gdFontGetTiny jonj_gd_gdFontGetTiny
#define gdFontGiant jonj_gd_gdFontGiant
#define gdFontGiantData jonj_gd_gdFontGiantData
#define gdFontGiantRep jonj_gd_gdFontGiantRep
#define gdFontLarge jonj_gd_gdFontLarge
#define gdFontLargeData jonj_gd_gdFontLargeData
#define gdFontLargeRep jonj_gd_gdFontLargeRep
#define gdFontMediumBold jonj_gd_gdFontMediumBold
#define gdFontMediumBoldData jonj_gd_gdFontMediumBoldData
#define gdFontMediumBoldRep jonj_gd_gdFontMediumBoldRep
#define gdFontSmall jonj_gd_gdFontSmall
#define gdFontSmallData jonj_gd_gdFontSmallData
#define gdFontSmallRep jonj_gd_gdFontSmallRep
#define gdFontTiny jonj_gd_gdFontTiny
#define gdFontTinyData jonj_gd_gdFontTinyData
#define gdFontTinyRep jonj_gd_gdFontTinyRep
#define gdGetBuf jonj_gd_gdGetBuf
#define gdGetByte jonj_gd_gdGetByte
#define gdGetC jonj_gd_gdGetC
#define _gdGetColors jonj_gd__gdGetColors
#define gd_getin jonj_gd_gd_getin
#define gdGetInt jonj_gd_gdGetInt
#define gdGetWord jonj_gd_gdGetWord
#define gdImageAABlend jonj_gd_gdImageAABlend
#define gdImageAALine jonj_gd_gdImageAALine
#define gdImageAlphaBlending jonj_gd_gdImageAlphaBlending
#define gdImageAntialias jonj_gd_gdImageAntialias
#define gdImageArc jonj_gd_gdImageArc
#define gdImageBrightness jonj_gd_gdImageBrightness
#define gdImageChar jonj_gd_gdImageChar
#define gdImageCharUp jonj_gd_gdImageCharUp
#define gdImageColor jonj_gd_gdImageColor
#define gdImageColorAllocate jonj_gd_gdImageColorAllocate
#define gdImageColorAllocateAlpha jonj_gd_gdImageColorAllocateAlpha
#define gdImageColorClosest jonj_gd_gdImageColorClosest
#define gdImageColorClosestAlpha jonj_gd_gdImageColorClosestAlpha
#define gdImageColorClosestHWB jonj_gd_gdImageColorClosestHWB
#define gdImageColorDeallocate jonj_gd_gdImageColorDeallocate
#define gdImageColorExact jonj_gd_gdImageColorExact
#define gdImageColorExactAlpha jonj_gd_gdImageColorExactAlpha
#define gdImageColorMatch jonj_gd_gdImageColorMatch
#define gdImageColorResolve jonj_gd_gdImageColorResolve
#define gdImageColorResolveAlpha jonj_gd_gdImageColorResolveAlpha
#define gdImageColorTransparent jonj_gd_gdImageColorTransparent
#define gdImageCompare jonj_gd_gdImageCompare
#define gdImageContrast jonj_gd_gdImageContrast
#define gdImageConvolution jonj_gd_gdImageConvolution
#define gdImageCopy jonj_gd_gdImageCopy
#define gdImageCopyMerge jonj_gd_gdImageCopyMerge
#define gdImageCopyMergeGray jonj_gd_gdImageCopyMergeGray
#define gdImageCopyResampled jonj_gd_gdImageCopyResampled
#define gdImageCopyResized jonj_gd_gdImageCopyResized
#define gdImageCreate jonj_gd_gdImageCreate
#define gdImageCreateFromGd jonj_gd_gdImageCreateFromGd
#define gdImageCreateFromGd2 jonj_gd_gdImageCreateFromGd2
#define gdImageCreateFromGd2Ctx jonj_gd_gdImageCreateFromGd2Ctx
#define gdImageCreateFromGd2Part jonj_gd_gdImageCreateFromGd2Part
#define gdImageCreateFromGd2PartCtx jonj_gd_gdImageCreateFromGd2PartCtx
#define gdImageCreateFromGd2PartPtr jonj_gd_gdImageCreateFromGd2PartPtr
#define gdImageCreateFromGd2Ptr jonj_gd_gdImageCreateFromGd2Ptr
#define gdImageCreateFromGdCtx jonj_gd_gdImageCreateFromGdCtx
#define gdImageCreateFromGdPtr jonj_gd_gdImageCreateFromGdPtr
#define gdImageCreateFromGif jonj_gd_gdImageCreateFromGif
#define gdImageCreateFromGifCtx jonj_gd_gdImageCreateFromGifCtx
#define gdImageCreateFromGifSource jonj_gd_gdImageCreateFromGifSource
#define gdImageCreateFromJpeg jonj_gd_gdImageCreateFromJpeg
#define gdImageCreateFromJpegCtx jonj_gd_gdImageCreateFromJpegCtx
#define gdImageCreateFromJpegPtr jonj_gd_gdImageCreateFromJpegPtr
#define gdImageCreateFromPng jonj_gd_gdImageCreateFromPng
#define gdImageCreateFromPngCtx jonj_gd_gdImageCreateFromPngCtx
#define gdImageCreateFromPngPtr jonj_gd_gdImageCreateFromPngPtr
#define gdImageCreateFromPngSource jonj_gd_gdImageCreateFromPngSource
#define gdImageCreateFromWBMP jonj_gd_gdImageCreateFromWBMP
#define gdImageCreateFromWBMPCtx jonj_gd_gdImageCreateFromWBMPCtx
#define gdImageCreateFromWBMPPtr jonj_gd_gdImageCreateFromWBMPPtr
#define gdImageCreateFromXbm jonj_gd_gdImageCreateFromXbm
#define gdImageCreatePaletteFromTrueColor jonj_gd_gdImageCreatePaletteFromTrueColor
#define gdImageCreateTrueColor jonj_gd_gdImageCreateTrueColor
#define gdImageDashedLine jonj_gd_gdImageDashedLine
#define gdImageDestroy jonj_gd_gdImageDestroy
#define gdImageEdgeDetectQuick jonj_gd_gdImageEdgeDetectQuick
#define gdImageEllipse jonj_gd_gdImageEllipse
#define gdImageEmboss jonj_gd_gdImageEmboss
#define gdImageFill jonj_gd_gdImageFill
#define gdImageFilledArc jonj_gd_gdImageFilledArc
#define gdImageFilledEllipse jonj_gd_gdImageFilledEllipse
#define gdImageFilledPolygon jonj_gd_gdImageFilledPolygon
#define gdImageFilledRectangle jonj_gd_gdImageFilledRectangle
#define _gdImageFillTiled jonj_gd__gdImageFillTiled
#define gdImageFillToBorder jonj_gd_gdImageFillToBorder
#define gdImageGaussianBlur jonj_gd_gdImageGaussianBlur
#define gdImageGd jonj_gd_gdImageGd
#define gdImageGd2 jonj_gd_gdImageGd2
#define gdImageGd2Ptr jonj_gd_gdImageGd2Ptr
#define gdImageGdPtr jonj_gd_gdImageGdPtr
#define gdImageGetClip jonj_gd_gdImageGetClip
#define gdImageGetPixel jonj_gd_gdImageGetPixel
#define gdImageGetTrueColorPixel jonj_gd_gdImageGetTrueColorPixel
#define gdImageGif jonj_gd_gdImageGif
#define gdImageGifCtx jonj_gd_gdImageGifCtx
#define gdImageGifPtr jonj_gd_gdImageGifPtr
#define gdImageGrayScale jonj_gd_gdImageGrayScale
#define gdImageInterlace jonj_gd_gdImageInterlace
#define gdImageJpeg jonj_gd_gdImageJpeg
#define gdImageJpegCtx jonj_gd_gdImageJpegCtx
#define gdImageJpegPtr jonj_gd_gdImageJpegPtr
#define gdImageLine jonj_gd_gdImageLine
#define gdImageMeanRemoval jonj_gd_gdImageMeanRemoval
#define gdImageNegate jonj_gd_gdImageNegate
#define gdImagePaletteCopy jonj_gd_gdImagePaletteCopy
#define gdImagePng jonj_gd_gdImagePng
#define gdImagePngCtx jonj_gd_gdImagePngCtx
#define gdImagePngCtxEx jonj_gd_gdImagePngCtxEx
#define gdImagePngEx jonj_gd_gdImagePngEx
#define gdImagePngPtr jonj_gd_gdImagePngPtr
#define gdImagePngPtrEx jonj_gd_gdImagePngPtrEx
#define gdImagePngToSink jonj_gd_gdImagePngToSink
#define gdImagePolygon jonj_gd_gdImagePolygon
#define gdImageRectangle jonj_gd_gdImageRectangle
#define gdImageRotate jonj_gd_gdImageRotate
#define gdImageRotate180 jonj_gd_gdImageRotate180
#define gdImageRotate270 jonj_gd_gdImageRotate270
#define gdImageRotate45 jonj_gd_gdImageRotate45
#define gdImageRotate90 jonj_gd_gdImageRotate90
#define gdImageSaveAlpha jonj_gd_gdImageSaveAlpha
#define gdImageSelectiveBlur jonj_gd_gdImageSelectiveBlur
#define gdImageSetAntiAliased jonj_gd_gdImageSetAntiAliased
#define gdImageSetAntiAliasedDontBlend jonj_gd_gdImageSetAntiAliasedDontBlend
#define gdImageSetBrush jonj_gd_gdImageSetBrush
#define gdImageSetClip jonj_gd_gdImageSetClip
#define gdImageSetPixel jonj_gd_gdImageSetPixel
#define gdImageSetStyle jonj_gd_gdImageSetStyle
#define gdImageSetThickness jonj_gd_gdImageSetThickness
#define gdImageSetTile jonj_gd_gdImageSetTile
#define gdImageSkewX jonj_gd_gdImageSkewX
#define gdImageSkewY jonj_gd_gdImageSkewY
#define gdImageSmooth jonj_gd_gdImageSmooth
#define gdImageString jonj_gd_gdImageString
#define gdImageString16 jonj_gd_gdImageString16
#define gdImageStringFT jonj_gd_gdImageStringFT
#define gdImageStringFTEx jonj_gd_gdImageStringFTEx
#define gdImageStringTTF jonj_gd_gdImageStringTTF
#define gdImageStringUp jonj_gd_gdImageStringUp
#define gdImageStringUp16 jonj_gd_gdImageStringUp16
#define gdImageTrueColorToPalette jonj_gd_gdImageTrueColorToPalette
#define gdImageWBMP jonj_gd_gdImageWBMP
#define gdImageWBMPCtx jonj_gd_gdImageWBMPCtx
#define gdImageWBMPPtr jonj_gd_gdImageWBMPPtr
#define gdImageXbmCtx jonj_gd_gdImageXbmCtx
#define gdNewDynamicCtx jonj_gd_gdNewDynamicCtx
#define gdNewDynamicCtxEx jonj_gd_gdNewDynamicCtxEx
#define gdNewFileCtx jonj_gd_gdNewFileCtx
#define gdNewSSCtx jonj_gd_gdNewSSCtx
#define gdPutBuf jonj_gd_gdPutBuf
#define gdPutC jonj_gd_gdPutC
#define _gdPutColors jonj_gd__gdPutColors
#define gdPutInt jonj_gd_gdPutInt
#define gd_putout jonj_gd_gd_putout
#define gdPutWord jonj_gd_gdPutWord
#define gdSeek jonj_gd_gdSeek
#define gdSinT jonj_gd_gdSinT
#define gd_strtok_r jonj_gd_gd_strtok_r
#define gdTell jonj_gd_gdTell
#define getmbi jonj_gd_getmbi
#define init_destination jonj_gd_init_destination
#define init_source jonj_gd_init_source
#define jpeg_gdIOCtx_dest jonj_gd_jpeg_gdIOCtx_dest
#define jpeg_gdIOCtx_src jonj_gd_jpeg_gdIOCtx_src
#define lsqrt jonj_gd_lsqrt
#define printwbmp jonj_gd_printwbmp
#define Putchar jonj_gd_Putchar
#define putmbi jonj_gd_putmbi
#define Putword jonj_gd_Putword
#define readwbmp jonj_gd_readwbmp
#define skipheader jonj_gd_skipheader
#define skip_input_data jonj_gd_skip_input_data
#define term_destination jonj_gd_term_destination
#define term_source jonj_gd_term_source
#define writewbmp jonj_gd_writewbmp
#define ZeroDataBlock jonj_gd_ZeroDataBlock
#define gdCacheCreate jonj_gd_gdCacheCreate
#define gdCacheDelete jonj_gd_gdCacheDelete
#define gdCacheGet jonj_gd_gdCacheGet
#define gdFontCacheSetup jonj_gd_gdFontCacheSetup
#define gdFontCacheShutdown jonj_gd_gdFontCacheShutdown
#define gdFreeFontCache jonj_gd_gdFreeFontCache
#endif /* HAVE_GD_BUNDLED */

/* Define to specify how much context to retain around the current parse
   point. */
#define XML_CONTEXT_BYTES 1024

/* Define to make parameter entity parsing functionality available. */
#define XML_DTD 1

/* Define to make XML Namespaces functionality available. */
#define XML_NS 1
#endif

#ifdef JONJ_EXPORTS
#define PCRE_STATIC
#endif

#endif
