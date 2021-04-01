#pragma once
// DWK for this entire file

// TODO, namespace?!
template <typename T>
class generator_saver
{
 public:
  explicit generator_saver
    ( const int gs_num
    , std::function<void*()> fn)
      : m_gs_num(gs_num)
      , m_fn(fn)
  {}

  generator_saver(generator_saver const &) = delete;
  generator_saver(generator_saver &&) = delete;

  ~generator_saver()
  {}

// TODO?
//  ~generator_saver()
//  {
//      if(m_saved)
//      {
//        delete m_saved;
//        m_saved = nullptr;
//      }
//  }

  void* do_it()
  {
    if(!m_saved)
    {
      if(!m_fn)
        return nullptr;
      m_saved = m_fn();
      // Created type
    }
    else
    {
      // Using previoulsy saved type
    }
    return m_saved;
  }
private:
  int m_gs_num {0};
  std::function<void*()> m_fn { [](){return nullptr;} };
  void* m_saved {nullptr};

};
