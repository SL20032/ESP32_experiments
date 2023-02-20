function replace_placeholder(Id_of_placeholder_element) {
  const Http = new XMLHttpRequest();
  const url='http://10.14.1.113/data_update_GET';

  Http.open("GET", url, true);
  Http.send();

  Http.onload = function() {

      const ID_of_placeholder_len = Id_of_placeholder_element.length;
      const http_response = Http.responseText;
      const http_resp_len = http_response.length;

      const ID_of_placeholder_pointer = http_response.indexOf(Id_of_placeholder_element) + ID_of_placeholder_len

      let placeholder_replacment_data_size
      if(http_response.indexOf('&',ID_of_placeholder_pointer) == -1)
      {
          placeholder_replacment_data_size = http_resp_len - ID_of_placeholder_pointer
      }
      else
      {
          placeholder_replacment_data_size = http_response.indexOf('&',ID_of_placeholder_pointer) - ID_of_placeholder_pointer
      }
      
      const placeholder_replacment_data = http_response.substring(ID_of_placeholder_pointer + 1, ID_of_placeholder_pointer + placeholder_replacment_data_size)

      const html_element_with_placeholder = document.getElementById(Id_of_placeholder_element).outerHTML
      
      document.getElementById(Id_of_placeholder_element).innerHTML = html_element_with_placeholder.replace('%PLH%',placeholder_replacment_data);
  }
}

function placholder_utility() {
  var url = location.protocol + '//' + location.host;
  console.log(url);
  
  replace_placeholder('test1')
  replace_placeholder('test2')
  replace_placeholder('test3')
}