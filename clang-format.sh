find -f ./tests -f ./include -f ./common -f ./benchmarks | grep -e ".*\.\h$" -e ".*\.\cpp$" | xargs clang-format -i