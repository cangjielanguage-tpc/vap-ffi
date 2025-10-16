/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package vap_cj

@C
public struct TagCharCp {
    var size: Int64
    var tagChar: CPointer<UInt8>

    init(size: Int64, tagChar: CPointer<UInt8>) {
    	this.size = size
        this.tagChar = tagChar
    }
}