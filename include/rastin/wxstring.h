#pragma once

#include <cereal/cereal.hpp>
#include <wx/string.h>
#include <wx/strvararg.h>  // для wxScopedCharBuffer

namespace cereal {

      // Сохранение wxString → UTF-8 (бинарно)
      template <class Archive>
      inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, wxString const& s)
      {
          // Получаем UTF-8 представление
          wxScopedCharBuffer utf8 = s.utf8_str();  // const char*, null-terminated
          size_t len = strlen(utf8.data());         // длина в байтах (без \0)

          // Записываем длину + данные (как сырые байты)
          ar(make_size_tag(static_cast<size_type>(len)));
          ar(binary_data(utf8.data(), len));
      }

      // Загрузка → из UTF-8 в wxString
      template <class Archive>
      inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, wxString& s)
      {
          size_type len;
          ar(make_size_tag(len));

          // Читаем в буфер
          std::vector<char> buffer(static_cast<size_t>(len));
          ar(binary_data(buffer.data(), static_cast<size_t>(len)));

          // Конструируем wxString из UTF-8
          s = wxString::FromUTF8(buffer.data(), static_cast<size_t>(len));
      }

  } // namespace cereal

