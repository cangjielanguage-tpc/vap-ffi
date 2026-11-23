/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package vap_cj
import std.unicode.UnicodeStringExtension

import std.convert.*
import std.collection.*
import std.collection.concurrent.*
import std.sync.*
import ohos.component.*
import ohos.system_date_time.SystemDateTime

@FastNative
foreign {
    func CangjiePluginRenderPlay(url: CString, mixDataCj: CPointer<CangjieMixData>, length: Int64,
        ptr: CFunc<() -> Unit>, id: Int64): Unit

    func CangjiePluginRenderGetVideoInfo(uri: CString, id: Int64): SrcInfoArray

    func CangjiePluginRenderSetFitType(fitTypeInt: Int64, id: Int64): Unit

    func CangjiePluginRenderPause(id: Int64): Unit

    func CangjiePluginRenderStop(id: Int64): Unit

    func CangjiePluginRenderSetPlayDone(id: Int64, value: Int32): Unit

    func CangjiePluginRenderIsPlayDone(id: Int64): Int32

    func CangjiePluginRenderSetVideoMode(videoModeInt: Int64, id: Int64): Unit

    func CangjiePluginRenderSetSpeed(speedInt: Float64, id: Int64): Unit

    func CangjiePluginRenderSetLoop(loop: Int64, id: Int64): Unit

    func CangjiePluginRenderOn(str: CString, cb: CFunc<(Int64, Int64) -> Unit>, surfaceId: Int64): Unit

    func GetsElementAnArray(buf: CPointer<CJBytesArray>, i: Int64): CJBytesArray

    func freeCJBytesArray(buf: CPointer<CJBytesArray>, i: Int64): Unit

    func FFIChangeColor(surfaceId: Int64): Unit

    func FFIDrawPattern(surfaceId: Int64): Unit

    func FFISetSurfaceId(surfaceId: Int64): Unit

    func FFIChangeSurface(surfaceId: Int64, width: Float32, height: Float32): Unit

    func FFIDestroySurface(surfaceId: Int64): Unit

    func FFIGetXComponentHasDraw(surface: Int64): Bool

    func FFIXComponentHasChangeColor(surface: Int64): Bool
}

public let loops: String = "Loops";

public var loopsMap: ConcurrentHashMap<String, VapController> = ConcurrentHashMap<String, VapController>()


public var playTimeMap: Map<Int64, Int64> = HashMap<Int64, Int64>()

@C
struct CJInt64Array{
    var size: Int64
    var buf: CPointer<CJBytesArray>

    init(size: Int64, buf: CPointer<CJBytesArray>) {
    	this.buf = buf
        this.size = size
    }
}

func cbFn(): Unit {
}

// ---- 播放完成跨线程方案（拉模型） ----
// 仓颉 @State 只允许主线程（UI 线程）更新，而播放完成回调 doneFn2 运行在 native 工作线程。
// 实测该运行时不存在可用的跨线程→主线程调度：直接调 UI 回调/Emitter 转投会落在非主线程
// （@State 主线程检查异常），spawn(Main) 无论从外来线程还是仓颉线程发起都会使主线程
// CJ_CJThreadContextInit 段错误崩溃。因此改为拉模型：doneFn2 只写 native 原子标志
// （setPlayDone），由 UI 页面用 Animator 帧回调（主线程动画管线驱动）轮询 isPlayDone
// 后在主线程更新 @State。
@C
func doneFn2(): Unit {
    let that: VapController = loopsMap[loops]
    that.loops.fetchSub(1)
    if (that.loops.load() > 0) {
        // 多循环重播：透传已保存的完成回调 jsCbfn，避免被模块级空 cbFn 覆盖，
        // 否则最后一轮播放结束后 UI 回调会丢失（如 buttonEnabled 无法恢复）
        if (let Some(v) <- that.jsCbfn) {
            that.play(that.uri, that.mixData, v)
        } else {
            that.play(that.uri, that.mixData, cbFn)
        }
    } else {
        that.loops.store(1)
        that.isRunning = false
        // 写 native 原子标志，主线程状态更新由 UI 页面的 Animator 帧回调轮询拉取
        that.setPlayDone(true)
    }
}

@C
func onStateChange(state: Int64, ret: Int64): Unit {
    Hilog.info(0, "VapController onStateChange", "VapController onStateChange-state: ${state},ret: ${ret}")
}

public class VapController <: XComponentController{
    public var loops: AtomicInt64 = AtomicInt64(1)
    public var isRunning: Bool = false
    public var jsCbfn: Option<() -> Unit> = None
    var downloadFile = DownloadFile()
    public var sandDir: String = ''
    public var uri: String = ''
    public var id: Int64 = 0
    public var pageName: String = ""
    public var url:String = ""
    public var downloadTimeout: Int64 = 60000
    public var downloadCallback: (data: DownLoadData) ->Unit = {data=>()}

    public func setGlobalLoops(): Unit {
        loopsMap["Loops"] = this
    }

    public var cbFn: () ->Unit = {=>()}
    public init(pageName: String) {
    	this.pageName = pageName
    }

    protected func onAnimationStart1(){
        cbFn()
    }

    public func setcallback(cbFn: () ->Unit):Unit {
        this.cbFn = cbFn
    }

    // 播放完成标志（native 侧原子存储）：供 UI 主线程的 Animator 帧回调轮询拉取
    public func setPlayDone(value: Bool): Unit {
        unsafe {
            CangjiePluginRenderSetPlayDone(id, if (value) { 1 } else { 0 })
        }
    }

    public func isPlayDone(): Bool {
        unsafe {
            CangjiePluginRenderIsPlayDone(id) == 1
        }
    }

    public func setUrl(url: String):Unit {
        this.url = url
    }

    public var mixData: ArrayList<CangjieMixData> = ArrayList<CangjieMixData>()

    protected  override func onSurfaceCreated(surfaceId: String): Unit{
        unsafe {
            id = Int64.parse(surfaceId)
            FFISetSurfaceId(id)
        }
    }

    protected  override func onSurfaceChanged(surfaceId: String, rect: SurfaceRect): Unit{
        unsafe {
            FFIChangeSurface(Int64.parse(surfaceId), rect.surfaceWidth, rect.surfaceHeight)
            var mixData: ArrayList<CangjieMixData> = ArrayList<CangjieMixData>()
            if(pageName.contains("NewOn")){
                setFitType(FitType.CENTER_CROP)
                setVideoMode(VideoMode.VIDEO_MODE_SPLIT_HORIZONTAL)
                this.play(this.url, mixData, this.onAnimationStart1)
            }
        }
    }

    protected  override func onSurfaceDestroyed(surfaceId: String): Unit{
        unsafe {
            FFIDestroySurface(Int64.parse(surfaceId))
        }
    }

    public var callback3: () -> Unit = {=>()}
    public func play(uri: String, mixData: ArrayList<CangjieMixData>, cbFn: () -> Unit) {
        unsafe {
            let mixDatas: Array<CangjieMixData> = getMixDatas(mixData)
            if (!mixData.isEmpty()) {
                this.mixData = ArrayList<CangjieMixData>()
                for (i in 0..mixData.size) {
                    if (!mixData[i].imgUri.toString().isEmpty()) {
                        var downloadFile: String = this
                            .downloadFile.downloadFile(mixData[i].imgUri.toString(), this.sandDir)
                        var item = mixData[i]
                        item.setImgUri(LibC.mallocCString(downloadFile))
                        mixData[i] = item
                    }
                    let cangjieMixData: CangjieMixData = CangjieMixData(mixData[i].tag.toString(),
                        mixData[i].txt.toString(), mixData[i].imgUri.toString(), mixData[i].colorA,
                        mixData[i].colorR, mixData[i].colorG, mixData[i].colorB,
                        mixData[i].vapFontWeight, mixData[i].vapTextAlign)
                    mixDatas[i] = cangjieMixData
                }
            }
            // TODO 获取Array<T>中的数据的原始指针实例
            var cptrHandle: CPointerHandle<CangjieMixData> = acquireArrayRawData(mixDatas)
            var cangjieData: CPointer<CangjieMixData> = cptrHandle.pointer

            this.uri = this.downloadFile.downloadFile(uri, this.sandDir)
            var cUri = LibC.mallocCString(uri)

            var mixDataArray = mixData.toArray()
            this.jsCbfn = cbFn
            setGlobalLoops()
            CangjiePluginRenderPlay(cUri, cangjieData, Int64(mixDatas.size), doneFn2, id)
        }
    }

    public func getMixDatas(mixData: ArrayList<CangjieMixData>): Array<CangjieMixData> {
        unsafe {
            var mixDatas: Array<CangjieMixData> = Array<CangjieMixData>(mixData.size,
                    repeat: CangjieMixData("", "", "", Int64(0), Int64(0), Int64(0), Int64(0), Int32(0), Int32(0)))
                if (!mixData.isEmpty()) {
                    this.mixData = ArrayList<CangjieMixData>()
                    for (i in 0..mixData.size) {
                        if (!mixData[i].imgUri.toString().isEmpty()) {
                            var downloadFile: String = this
                                .downloadFile.downloadFile(mixData[i].imgUri.toString(), this.sandDir)
                            var item = mixData[i]
                            item.setImgUri(LibC.mallocCString(downloadFile))
                            mixData[i] = item
                        }
                        let cangjieMixData: CangjieMixData = CangjieMixData(mixData[i].tag.toString(),
                            mixData[i].txt.toString(), mixData[i].imgUri.toString(), mixData[i].colorA,
                            mixData[i].colorR, mixData[i].colorG, mixData[i].colorB,
                            mixData[i].vapFontWeight, mixData[i].vapTextAlign)
                        mixDatas[i] = cangjieMixData
                    }
                }
            return mixDatas;
       }
    }

    public var callback2: (Int64, Int64) -> Unit = {state, ret =>()}

    public func on(str: String, callback: (Int64, Int64) -> Unit): Unit {
        callback2 = callback
        unsafe {
            CangjiePluginRenderOn(LibC.mallocCString(str), onStateChange, id)
        }
    }

    public func pause(): Unit {
        unsafe {
            CangjiePluginRenderPause(id)
        }
    }

    public func stop(): Unit {
        unsafe {
            CangjiePluginRenderStop(id)
            this.loops.store(1)
        }
    }

    public func setLoop(loops: Int64): Unit {
        this.loops.store(loops)
        setGlobalLoops()
        unsafe {
            CangjiePluginRenderSetLoop(this.loops.load(), id)
        }
    }

    public func setDownloadAttribute(dTime: Int64, downloadProgress: (data:DownLoadData) -> Unit): Unit {
        this.downloadTimeout = dTime
        this.downloadCallback = downloadProgress
    }

    public func getVideoInfo(uri: String): Array<SrcInfo> {
        unsafe {
            var cUri = LibC.mallocCString(uri)
            var cs: SrcInfoArray = CangjiePluginRenderGetVideoInfo(cUri, id)
            let size: Int64 = cs.size
            let srcInfoArray: Array<SrcInfo> = Array<SrcInfo>(size, repeat: SrcInfo(0,"",0,0,0))
            let srcIdArray: Array<Int64> = Array<Int64>(size, repeat: 0)
            let srcTypeArray: Array<Int64> = Array<Int64>(size, repeat: 0)
            let tagArray: Array<CString> = Array<CString>(size, repeat: LibC.mallocCString(""))
            for(i in 0..size){
                let newSrcInfo = SrcInfo(0,"",0,0,0)
                let srcIdInt64: CPointer<Int64> = cs.srcIdInt64
                let srcTypeInt64: CPointer<Int64> = cs.srcTypeInt64
                let res: CJInt64Array = cs.tagChar
                let size: Int64 = res.size
                let buf: CPointer<CJBytesArray> = res.buf
                let bytes: Array<CJBytesString> = Array<CJBytesString>(size, repeat: CJBytesString(0, ""))
                if(srcIdInt64.isNotNull()){
                    srcIdArray[i] = srcIdInt64.read(i)
                    newSrcInfo.srcId = srcIdInt64.read(i)
                }
                if(srcTypeInt64.isNotNull()){
                    srcIdArray[i] = srcTypeInt64.read(i)
                    newSrcInfo.srcType = srcTypeInt64.read(i)
                }

                var newBuf: CJBytesArray = GetsElementAnArray(res.buf, i)
                var tag: String = BytesArrayToString(newBuf)
                newSrcInfo.tag = tag
                newSrcInfo.width = cs.width
                newSrcInfo.height = cs.height
                srcInfoArray[i] = newSrcInfo
                freeCJBytesArray(res.buf, i)
            }
            return srcInfoArray
        }
    }

    public func setFitType(fitType: FitType): Unit {
        unsafe {
            var fitTypeInt64: Int64 = getFitType(fitType)
            CangjiePluginRenderSetFitType(fitTypeInt64, id)
        }
    }

    public func setVideoMode(videoMode: VideoMode): Unit {
        unsafe {
            var videoModeInt64 = getVideoMode(videoMode)
            CangjiePluginRenderSetVideoMode(videoModeInt64, id)
        }
    }

    public func setSpeed(speed: VAPSpeed): Unit {
        unsafe {
            var speedInt64 = getVAPSpeed(speed)
            CangjiePluginRenderSetSpeed(speedInt64, id)
        }
    }

    public func getFitType(that: FitType): Int64 {
        match (that) {
            case FIT_XY => 0
            case FIT_CENTER => 1
            case _ => 2
        }
    }

    public func getVideoMode(that: VideoMode): Int64 {
        match (that) {
            case VIDEO_MODE_SPLIT_HORIZONTAL => 0
            case VIDEO_MODE_SPLIT_VERTICAL => 1
            case VIDEO_MODE_SPLIT_HORIZONTAL_REVERSE => 2
            case _ => 3
        }
    }

    public func getVAPSpeed(that: VAPSpeed): Float64 {
        match (that) {
            case VAP_Speed_1_00_X => 1.0
            case VAP_Speed_0_50_X => 0.5
            case VAP_Speed_1_50_X => 1.5
            case _ => 2.0
        }
    }
}
