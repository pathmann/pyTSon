#ifndef __SINGLETON__H
#define __SINGLETON__H

template <typename T>
class singleton {
  public:
    static T* instance() {
      return ((m_watcher.m_object) ? m_watcher.m_object : m_watcher.m_object = new T);
    }

    static void clip() {
      instance();

      if (m_clip.m_object)
        return;

      m_clip.m_object = new T(*m_watcher.m_object);
    }

    static void accept() {
      if (!m_clip.m_object)
        return;

      delete m_clip.m_object;
    }

    static void revert() {
      if (!m_clip.m_object)
        return;

      T* tmp = m_watcher.m_object;
      m_watcher.m_object = m_clip.m_object;

      delete tmp;
      m_clip.m_object = 0;
    }
  private:
    struct watcher {
      T* m_object;
      watcher() {
        m_object = 0;
      }

      ~watcher() {
        if (m_object)
          delete m_object;
      }
    };

    static watcher m_watcher;
    static watcher m_clip;
  protected:
    singleton() {}
    ~singleton() {}
};

template <typename T> typename singleton<T>::watcher singleton<T>::m_watcher;
template <typename T> typename singleton<T>::watcher singleton<T>::m_clip;

#endif // __SINGLETON__H
