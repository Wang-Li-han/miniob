/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "common/type/data_type.h"
#include "common/sys/rc.h"

/**
 * @brief 日期类型
 * @details 内部以 4 字节 int 存储 YYYYMMDD（例如 2026-10-16 编码为 20261016）。
 *          不使用 time_t，规避 2038 年溢出问题；支持 1970 年之前以及 2038 年之后。
 * @ingroup DataType
 */
class DateType : public DataType
{
public:
  DateType() : DataType(AttrType::DATES) {}
  virtual ~DateType() {}

  int compare(const Value &left, const Value &right) const override;

  RC cast_to(const Value &val, AttrType type, Value &result) const override;

  int cast_cost(AttrType type) override
  {
    if (type == AttrType::DATES) {
      return 0;
    }
    return INT32_MAX;
  }

  RC set_value_from_str(Value &val, const string &data) const override;

  RC to_string(const Value &val, string &result) const override;

  /**
   * @brief 将 YYYY-MM-DD 字符串解析并校验为日期编码(YYYYMMDD)
   * @param s        输入字符串
   * @param date_val 输出：日期编码整数
   * @return SUCCESS 合法；FAILURE 非法（格式错误/月份/天数/闰年等）
   */
  static RC str_to_date(const char *s, int &date_val);
};
