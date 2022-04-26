#include <curl/curl.h>
#include <json/json.h>

struct string {
    char* ptr;
    size_t len;
};

void init_string(struct string* s) {
    s->len = 0;
    s->ptr = (char*)malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writeResponseData(void* ptr, size_t size, size_t nmemb, struct string* s)
{
    size_t new_len = s->len + size * nmemb;
    s->ptr = (char*)realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

int main(void)
{
    CURL* curl;
    CURLcode res;

    struct curl_slist* headerlist = nullptr;
    headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

    std::string strResourceJSON = "{\"user\":\"mopic1\",\"key\":\"test_key1\"}";    // success
    //std::string strResourceJSON = "{\"user\":\"mopic2\",\"key\":\"test_key2\"}";  // fail : expire
    //std::string strResourceJSON = "{\"user\":\"mopic3\",\"key\":\"test_key2\"}";  // fail : invalid key

    curl = curl_easy_init();
    if (curl)
    {
        struct string s;
        init_string(&s);

        // request set
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3001/validUser");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strResourceJSON.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

        // response set
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponseData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        // run request
        res = curl_easy_perform(curl);

        // success & errors
        if (res == CURLE_OK)
        {
            printf("응답받은 데이터 : %s\n", s.ptr);
        }
        else
        {
            printf("curl_easy_perform() fail: %s\n", curl_easy_strerror(res));
        }

        // cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headerlist);

        free(s.ptr);
    }

    system("pause");

    return 0;
}
