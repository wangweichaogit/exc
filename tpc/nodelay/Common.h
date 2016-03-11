#pragma once
#include <string>

//������   ����=delete��˼��delete�������˺������ã��������������copy����͸�ֵ���������
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

	//������ ���������ڲ����ı����ֵ
  const char* c_str() const { return str_; }

 private:
  const char* str_;
};

//�������߱�����ִ��ʲô��ʽ����ת��
template<typename To, typename From>
inline To implicit_cast(const From &f)
{
  return f;
}
