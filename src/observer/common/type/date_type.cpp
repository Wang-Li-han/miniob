/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/type/date_type.h"

#include <cstdio>
#include <cstring>

#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/value.h"

// 判断是否闰年
static bool is_leap_year(int year)
{
  if (year % 400 == 0) {
    return true;
  }
  if (year % 100 == 0) {
    return false;
  }
  return (year % 4 == 0);
}

// 获取某年某月最大天数
static int get_month_max_day(int year, int month)
{
  static const int month_days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && is_leap_year(year)) {
    return 29;
  }
  return month_days[month];
}

RC DateType::str_to_date(const char *s, int &date_val)
{
  if (s == nullptr || s[0] == '\0') {
    return RC::INVALID_ARGUMENT;
  }

  int year  = 0;
  int month = 0;
  int day   = 0;

  // 严格匹配 YYYY-MM-DD
  int matched = sscanf(s, "%d-%d-%d", &year, &month, &day);
  if (matched != 3) {
    return RC::INVALID_ARGUMENT;
  }

  // 基础范围校验
  if (year <= 0 || month < 1 || month > 12 || day < 1) {
    return RC::INVALID_ARGUMENT;
  }

  int max_day = get_month_max_day(year, month);
  if (day > max_day) {
    return RC::INVALID_ARGUMENT;
  }

  // 合法日期，编码为 YYYYMMDD
  date_val = year * 10000 + month * 100 + day;
  return RC::SUCCESS;
}

int DateType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::DATES, "left type is not date");
  if (right.attr_type() == AttrType::DATES) {
    return common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
  }

  if (right.attr_type() == AttrType::CHARS) {
    int right_date = 0;
    RC  rc         = DateType::str_to_date(right.get_string().c_str(), right_date);
    if (rc != RC::SUCCESS) {
      // 右值不是合法日期，返回 INT32_MAX 表示无法比较（行会被过滤）
      return INT32_MAX;
    }
    return common::compare_int((void *)&left.value_.int_value_, (void *)&right_date);
  }

  return INT32_MAX;
}

RC DateType::cast_to(const Value &val, AttrType type, Value &result) const
{
  // DATE 到其他类型的转换暂不支持（题目不要求）
  return RC::UNSUPPORTED;
}

RC DateType::set_value_from_str(Value &val, const string &data) const
{
  int date_val = 0;
  RC  rc       = DateType::str_to_date(data.c_str(), date_val);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  val.set_type(AttrType::DATES);
  val.value_.int_value_ = date_val;
  val.length_           = sizeof(int);
  return RC::SUCCESS;
}

RC DateType::to_string(const Value &val, string &result) const
{
  int  code = val.value_.int_value_;
  int  y    = code / 10000;
  int  m    = (code % 10000) / 100;
  int  d    = code % 100;
  char buf[32];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d", y, m, d);
  result = buf;
  return RC::SUCCESS;
}
