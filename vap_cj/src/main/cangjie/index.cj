/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package vap_cj

@C
public struct CangjieMixData {
    public var tag: CString
    public var txt: CString
    public var imgUri: CString
    public var colorA: Int64 = 255
    public var colorR: Int64 = 2
    public var colorG: Int64 = 255
    public var colorB: Int64 = 187
    public var vapFontWeight: Int32 = 0
    public var vapTextAlign: Int32 = 0

    public init(tag: String, txt: String, imgUri: String, colorA: Int64, colorR: Int64, colorG: Int64, colorB: Int64) {
        unsafe {
            this.tag = LibC.mallocCString(tag)
            this.txt = LibC.mallocCString(txt)
            this.imgUri = LibC.mallocCString(imgUri)
            this.colorA = colorA
            this.colorR = colorR
            this.colorG = colorG
            this.colorB = colorB
        }
    }

    public init(tag: String, txt: String, imgUri: String, colorA: Int64, colorR: Int64, colorG: Int64, colorB: Int64,
        vapFontWeight: Int32, vapTextAlign: Int32) {
        unsafe {
            this.tag = LibC.mallocCString(tag)
            this.txt = LibC.mallocCString(txt)
            this.imgUri = LibC.mallocCString(imgUri)
            this.colorA = colorA
            this.colorR = colorR
            this.colorG = colorG
            this.colorB = colorB
            this.vapFontWeight = vapFontWeight
            this.vapTextAlign = vapTextAlign
        }
    }

    public init(tag: String, txt: String, imgUri: String) {
        unsafe {
            this.tag = LibC.mallocCString(tag)
            this.txt = LibC.mallocCString(txt)
            this.imgUri = LibC.mallocCString(imgUri)
        }
    }

    public mut func setImgUri(str: CString):Unit {
        unsafe {
            this.imgUri = str
        }
    }
}

@C
public struct CJMixData {
    var tag: CString = unsafe {LibC.mallocCString("")}
    var txt: CString = unsafe {LibC.mallocCString("")}
    var imgUri: CString = unsafe {LibC.mallocCString("")}
    var color: UInt32 = 0
    var fontWeight: UInt64 = 0
    var textAlign: UInt64 = 0
}

@C
public struct CJMixDataArray {
    var size: Int64
    var buf: CPointer<CJMixData>

    init(size: Int64, buf: CPointer<CJMixData>) {
    	this.buf = buf
        this.size = size
    }
}