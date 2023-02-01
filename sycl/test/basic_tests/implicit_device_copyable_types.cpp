// RUN: %clangxx -fsycl -fsycl-targets=%sycl_triple %s -o %t.out

#include <sycl/sycl.hpp>
#include <variant>

int main() {
    sycl::queue q;
    {
        std::pair<int, float> pair_arr[5];
        q.submit([&](sycl::handler &cgh) {
           cgh.single_task([=]() { std::pair<int, float> p0 = pair_arr[0]; });
         }).wait_and_throw();
    }

    {
        std::tuple<int, float, bool> tuple_arr[5];
        q.submit([&](sycl::handler &cgh) {
           cgh.single_task(
               [=]() { std::tuple<int, float, bool> t0 = tuple_arr[0]; });
         }).wait_and_throw();
    }

    {
        std::variant<int, float, bool> variant_arr[5];
        q.submit([&](sycl::handler &cgh) {
           cgh.single_task(
               [=]() { std::variant<int, float, bool> v0 = variant_arr[0]; });
         }).wait_and_throw();
    }

    {
        std::array<int, 513> arr[5];
        q.submit([&](sycl::handler &cgh) {
           cgh.single_task([=]() { std::array<int, 513> arr0 = arr[0]; });
         }).wait_and_throw();
    }

    {
        sycl::queue q{};
        std::optional<int> opt[5];
        q.submit([&](sycl::handler &cgh) {
           cgh.single_task([=]() { std::optional<int> o0 = opt[0]; });
         }).wait_and_throw();
    }

    {
        std::string_view strv_arr[5];
        q.submit([&](sycl::handler &cgh) {
           cgh.single_task([=]() { std::string_view str0 = strv_arr[0]; });
         }).wait_and_throw();
    }

    {
        // TODO: Enable when span is available
        // std::vector<int> v(5);
        // std::span s{a, a + 4};
        // q.submit([&](sycl::handler &cgh) {
        //   cgh.single_task([=]() { int x = s[0]; });
        // }).wait_and_throw();
    }

  return 0;
}
