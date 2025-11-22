/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package vap_cj

let NullArray = Array<SrcInfo>()

public enum VAPFontWeight {
    | THIN
    | EXTRA_LIGHT
    | LIGHT
    | NORMAL
    | MEDIUM
    | SEMI_BOLD
    | BOLD
    | EXTRA_BOLD
    | BLACK

    public func getFontWeight(that: Int32): VAPFontWeight {
        match (that) {
            case 0 => THIN
            case 1 => EXTRA_LIGHT
            case 2 => LIGHT
            case 3 => NORMAL
            case 4 => MEDIUM
            case 5 => SEMI_BOLD
            case 6 => BOLD
            case 7 => EXTRA_BOLD
            case _ => BLACK
        }
    }

    public func getFontWeightF(that: VAPFontWeight): Int32 {
        match (that) {
            case THIN => 0
            case EXTRA_LIGHT => 1
            case LIGHT => 2
            case NORMAL => 3
            case MEDIUM => 4
            case SEMI_BOLD => 5
            case BOLD => 6
            case EXTRA_BOLD => 7
            case _ => 8
        }
    }
}

public enum VAPTextAlign {
    | TEXT_ALIGN_LEFT
    | TEXT_ALIGN_RIGHT
    | TEXT_ALIGN_CENTER
    | TEXT_ALIGN_JUSTIFY
    | TEXT_ALIGN_START
    | TEXT_ALIGN_END

    public func getVAPTextAlign(that: Int32): VAPTextAlign {
        match (that) {
            case 0 => TEXT_ALIGN_LEFT
            case 1 => TEXT_ALIGN_RIGHT
            case 2 => TEXT_ALIGN_CENTER
            case 3 => TEXT_ALIGN_JUSTIFY
            case 4 => TEXT_ALIGN_START
            case _ => TEXT_ALIGN_END
        }
    }

    public func getVAPTextAlignF(that: VAPTextAlign): Int32 {
        match (that) {
            case TEXT_ALIGN_LEFT => 0
            case TEXT_ALIGN_RIGHT => 1
            case TEXT_ALIGN_CENTER => 2
            case TEXT_ALIGN_JUSTIFY => 3
            case TEXT_ALIGN_START => 4
            case _ => 5
        }
    }
}

public enum FitType {
    | FIT_XY
    | FIT_CENTER
    | CENTER_CROP

    public func getFitType(that: Int32): FitType {
        match (that) {
            case 0 => FIT_XY
            case 1 => FIT_CENTER
            case _ => CENTER_CROP
        }
    }

    public func getFitType(that: FitType): Int32 {
        match (that) {
            case FIT_XY => 0
            case FIT_CENTER => 1
            case _ => 2
        }
    }
}

public enum VapState {
    | UNKNOWN
    | READY
    | START
    | RENDER
    | COMPLETE
    | DESTROY
    | FAILED

    public func getVapState(that: Int64): VapState {
        match (that) {
            case 0 => UNKNOWN
            case 1 => READY
            case 2 => START
            case 3 => RENDER
            case 4 => COMPLETE
            case 5 => DESTROY
            case _ => FAILED
        }
    }

    public func getVapState(that: VapState): Int64 {
        match (that) {
            case UNKNOWN => 0
            case READY => 1
            case START => 2
            case RENDER => 3
            case COMPLETE => 4
            case DESTROY => 5
            case _ => 6
        }
    }
}

public enum Orien {
    | ORIEN_DEFAULT // 兼容模式
    | ORIEN_PORTRAIT // 适配竖屏的视频
    | ORIEN_LANDSCAPE // 适配横屏的视频

    public operator func ==(that: Orien): Int32 {
        match ((this, that)) {
            case (ORIEN_DEFAULT, ORIEN_DEFAULT) => 0
            case (ORIEN_PORTRAIT, ORIEN_PORTRAIT) => 1
            case (ORIEN_LANDSCAPE, ORIEN_LANDSCAPE) => 2
            case _ => 0
        }
    }
}

public enum SrcType {
    | UNKNOWN
    | IMG
    | TXT

    public func getSrcType(that: SrcType): String {
        match (that) {
            case UNKNOWN => "UNKNOWN"
            case IMG => "IMG"
            case _ => "TXT"
        }
    }

    public func getSrcTypeInt(that: SrcType): Int64 {
        match (that) {
            case UNKNOWN => 1
            case IMG => 2
            case _ => 3
        }
    }
}

public enum VideoMode {
    // 视频对齐方式 (兼容老版本视频模式)
    | VIDEO_MODE_SPLIT_HORIZONTAL // 视频左右对齐（alpha左\rgb右）
    | VIDEO_MODE_SPLIT_VERTICAL // 视频上下对齐（alpha上\rgb下）
    | VIDEO_MODE_SPLIT_HORIZONTAL_REVERSE // 视频左右对齐（rgb左\alpha右）
    | VIDEO_MODE_SPLIT_VERTICAL_REVERSE // 视频上下对齐（rgb上\alpha下）
}

public enum VAPSpeed {
    | VAP_Speed_1_00_X
    | VAP_Speed_0_50_X
    | VAP_Speed_1_50_X
    | VAP_Speed_2_00_X

    public operator func ==(that: VAPSpeed): Float32 {
        match ((this, that)) {
            case (VAP_Speed_1_00_X, VAP_Speed_1_00_X) => 1.0
            case (VAP_Speed_0_50_X, VAP_Speed_0_50_X) => 0.5
            case (VAP_Speed_1_50_X, VAP_Speed_1_50_X) => 1.5
            case _ => 2.0
        }
    }
}

public class DownLoadData {
    public var receiveSize: Int64 = 0
    public var totalSize: Int64 = 0

    public func getDownLoadData(receiveSize: Int64, totalSize: Int64):Unit {
        this.receiveSize = receiveSize
        this.totalSize = totalSize
    }
}

@C
struct SrcInfoArray{
    var size: Int64
    var srcIdInt64: CPointer<Int64>
    var srcTypeInt64: CPointer<Int64>
    var tagChar: CJInt64Array
    var width: Int32
    var height: Int32

    init(size: Int64, srcIdInt64: CPointer<Int64>, srcTypeInt64: CPointer<Int64>, tagChar: CJInt64Array,
        width: Int32, height: Int32) {
    	this.size = size
        this.srcIdInt64 = srcIdInt64
        this.srcTypeInt64 = srcTypeInt64
        this.tagChar = tagChar
        this.width = width
        this.height = height
    }
}

@C
struct CJMarker{
    var size: Int64
    var cJMarker: CPointer<SrcInfoo>

    init(size: Int64, cJMarker: CPointer<SrcInfoo>) {
    	this.size = size
        this.cJMarker = cJMarker
    }
}

@C
struct CJBytesArray{
    var size: Int64
    var buf: CPointer<UInt8>

    init(size: Int64, buf: CPointer<UInt8>) {
    	this.size = size
        this.buf = buf
    }
}

public class CJBytesString{
    public let size: Int64
    public let buf: String

    public CJBytesString(size: Int64, buf: String) {
    	this.size = size
        this.buf = buf
    }
}

@C
struct SrcInfoo{
    var srcId: Int64
    var tag: CPointer<UInt8>
    var srcType: Int64
    var width: Int32
    var height: Int32

    init(srcId: Int64, tag: CPointer<UInt8>, srcType: Int64, width: Int32, height: Int32) {
    	this.srcId = srcId
        this.tag = tag
        this.srcType = srcType
        this.width = width
        this.height = height
    }
}

public class SrcInfo{
    public var srcId: Int64
    public var tag: String
    public var srcType: Int64
    public var width: Int32
    public var height: Int32

    init(srcId: Int64, tag: String, srcType: Int64, width: Int32, height: Int32) {
    	this.srcId = srcId
        this.tag = tag
        this.srcType = srcType
        this.width = width
        this.height = height
    }
}

public class VideoInfo {
    public var width: Int32 = 0
    public var height: Int32 = 0
    public var srcInfos: Array<SrcInfo> = NullArray

    init(width: Int32, height: Int32, srcInfos: Array<SrcInfo>) {
        this.width = width;
        this.height = height;
        this.srcInfos = srcInfos;
    }
}

func BytesArrayToString(res: CJBytesArray): String{
    unsafe {
        let size: Int64 = res.size
        if(size == 0){
            return "";
        }
        let buf: CPointer<UInt8> = res.buf
        let bytes: Array<UInt8> = Array<UInt8>(size, repeat: 0)
        for(i in 0..size){
            if(buf.isNotNull()){
                bytes[i] = buf.read(i)
            }
        }
        let str = String.fromUtf8(bytes)
        return str
    }
}

public class VAPColor {
    public var a: Int32 = 0
    public var r: Int32 = 0
    public var g: Int32 = 0
    public var b: Int32 = 0
}
