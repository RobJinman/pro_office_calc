#ifndef __PROCALC_QT_OBJ_PTR_HPP__
#define __PROCALC_QT_OBJ_PTR_HPP__


#include <functional>
#include <memory>
#include <QPointer>


template <class T>
using CustomQtObjDeleter = std::function<void(const T*)>;

template <class T>
using QtObjPtr = std::unique_ptr<T, CustomQtObjDeleter<T>>;

template <class T, typename... Args>
auto makeQtObjPtr(Args&&... args) {
  // Instantiate the object in an exception-safe way
  auto tmp = std::make_unique<T>(std::forward<Args>(args)...);

  // A QPointer will keep track of whether the object is still alive
  QPointer<T> qPtr(tmp.get());

  CustomQtObjDeleter<T> deleter = [qPtr](const T*) {
    if (qPtr) {
      qPtr->deleteLater();
    }
  };

  return QtObjPtr<T>(tmp.release(), std::move(deleter));
}

template <class T>
auto makeQtObjPtrFromRawPtr(T* rawPtr) {
  QPointer<T> qPtr(rawPtr);

  CustomQtObjDeleter<T> deleter = [qPtr](const T*) {
    if (qPtr) {
      qPtr->deleteLater();
    }
  };

  return QtObjPtr<T>(rawPtr, std::move(deleter));
}


#endif