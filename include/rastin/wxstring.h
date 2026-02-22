#pragma once

#include <cereal/cereal.hpp>
#include <wx/string.h>
#include <wx/strvararg.h>

namespace cereal {

      template <class Archive>
      inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, wxString const& s)
      {
          wxScopedCharBuffer utf8 = s.utf8_str();  
          size_t len = strlen(utf8.data());         

          ar(make_size_tag(static_cast<size_type>(len)));
          ar(binary_data(utf8.data(), len));
      }
      template <class Archive>
      inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, wxString& s)
      {
          size_type len;
          ar(make_size_tag(len));

          std::vector<char> buffer(static_cast<size_t>(len));
          ar(binary_data(buffer.data(), static_cast<size_t>(len)));

          s = wxString::FromUTF8(buffer.data(), static_cast<size_t>(len));
      }

  } // namespace cereal

