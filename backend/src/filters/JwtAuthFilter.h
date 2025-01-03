#pragma once
#include <drogon/HttpFilter.h>
#include <jwt-cpp/jwt.h>

class JwtAuthFilter : public drogon::HttpFilter<JwtAuthFilter> {
private:
    static const std::string JWT_SECRET;
public:
    virtual void doFilter(const drogon::HttpRequestPtr& req,
                         drogon::FilterCallback&& fcb,
                         drogon::FilterChainCallback&& fccb) override;
};
