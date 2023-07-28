
# For testing Google Vision API
# 06/25

# call Vision api on terminal
# curl -X POST -k --location "https://35.194.249.251/vision" \
#   --header "Authorization: Bearer %_put_your_auth_token_here_%" \
#   --form "image=@sign.jpg"



# from asyncio.windows_events import NULL
import requests
import torch
import re

# string = "The date is 2023-06-30, and it is a sunny day."
pattern = r"\d{4}-\d{2}-\d{2}"





headers = {
    'Authorization': 'Bearer %_put_your_auth_token_here_%',
}

files = {
    'image': open('image.jpg', 'rb'),
}

def exp_time_api():
    response = requests.post('https://35.194.249.251/vision', headers=headers, files=files, verify=False)

    # print(response.text)
    print(type(response.json()))

    result = response.json()
    # result['data'] is the output of the vision api

    # store all the detected text into a list
    detect_text_list = []
    for _ in result['data']:
        detect_text_list.append(_['description'])
        
    print(detect_text_list)
    # print((result['data'][0]))
    if(len(detect_text_list) > 0):
        for string in detect_text_list:
            matches = re.findall(pattern, string)
            if matches:
                date = matches[0]
                print("Extracted date:", date)
                return date
            
        return None
    else:
        return None
    return detect_text_list

# exp_time_api()
# print(torch.__version__)