#pragma once
#include <string>

//单例类   函数=delete意思是delete函数（此函数禁用），单例类禁用了copy构造和赋值运算符重载
class noncopyable
{
 protected:
  noncopyable() {}

 private:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;
};

struct copyable
{
};

// For passing C-style string argument to a function.
class StringArg : copyable
{
 public:
  StringArg(const char* str)
    : str_(str)
  { }

  StringArg(const std::string& str)
    : str_(str.c_str())
  { }

	//常函数 声明函数内部不改变变量值
  const char* c_str() const { return str_; }

 private:
  const char* str_;
};

//用来告诉编译器执行什么隐式类型转换
template<typename To, typename From>
inline To implicit_cast(const From &f)
{
  return f;
}
