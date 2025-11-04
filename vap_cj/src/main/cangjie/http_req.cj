/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package vap_cj

internal import ohos.hilog.Hilog
import std.regex.*
import ohos.net.http.*

public class DownloadFile {
    public func downloadFile(url: String, dir: String): String {
        if (!startsWithHttp(url)) {
            return url
        }
        var fileName = extractFileName(url)
        var filePath: String = '/storage/Users/currentUser/Documents/' + fileName
        if (!dir.isEmpty()) {
            filePath = dir + '/' + fileName
        }
        let httpRequest = createHttp()
        let option = HttpRequestOptions(
            method: RequestMethod.GET,
            expectDataType: HttpDataType.ARRAY_BUFFER
        )
        let x: Array<UInt8>
        httpRequest.request(
            filePath,
            {
                err, resp =>
                if (let Some(e) <- err) {

                    Hilog.error(0, "DownloadFile downloadFile err", "exception: ${e.message}")
                }
                if (let Some(r) <- resp) {
                    Hilog.error(0, "DownloadFile downloadFile resp", "${r}")
                    let data = r.result as Array<UInt8>
                } else {
                    Hilog.error(0, "DownloadFile downloadFile resp is none", "resp is none")
                }
                httpRequest.destroy()
            },
            options: option
        )
        return ""
    }

    public func extractFileName(url: String): String {
        // 使用正则表达式匹配 URL 中的文件名部分
        let rule: Regex = Regex("/\\/([^\\/?#]+)[^\\/]*$/")
        if (rule.matcher(url).find().isSome()) {
            let matcher = rule.matcher(url)
            var arr: Array<MatchData> = matcher.findAll() ?? Array<MatchData>()
            return arr[0].matchString(Int64(1))
        }
        return ""
    }

    public func startsWithHttp(url: String): Bool {
        return url.startsWith("http") || url.startsWith("https")
    }


    public func writeFile(filePath: String, buf: Array<UInt8>): Unit {
        //let file = fs.openSync(filePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
        //  fs.writeSync(file.fd, buf);
        //  fs.closeSync(file);

    }
}
