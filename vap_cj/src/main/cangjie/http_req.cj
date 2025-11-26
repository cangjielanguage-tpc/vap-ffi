/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package vap_cj

internal import ohos.hilog.Hilog
import std.regex.*
import ohos.net.http.*
import std.fs.Path

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
        let rule: Regex = Regex("[^\\/]+\\.[^\\/?#]+(?=[\\/?#]|$)")
        var matches = rule.matches(url);
        if (matches) {
            let path: Path = Path(url)
            return path.fileName;
        }
        // 如果没有匹配到，则返回空字符串或者你想要的默认值
        return '';
    }

    public func startsWithHttp(url: String): Bool {
        return url.startsWith("http") || url.startsWith("https")
    }

    public func writeFile(filePath: String, buf: Array<UInt8>): Unit {
        Hilog.info(0, "writeFile  filePath", "writeFile  filePath: ${filePath}, buf size: ${buf.size}")
    }
}
