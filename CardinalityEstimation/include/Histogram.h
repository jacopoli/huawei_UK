#include <common/Root.h>

class Histogram {
private:
    std::vector<int> prefix_sum;
    int bucket_count;
    int max_value;
    int min_value;

public:
    Histogram(int bucket_count, int max_value, int min_value)
        : bucket_count(bucket_count), max_value(max_value), min_value(min_value)
    {
        prefix_sum.resize(bucket_count + 1, 0);
    }

    void add(int value)
    {
        int index = bucket_count * ((value - min_value) * 1.0 / (max_value - min_value));
        if (index >= 0 && index < bucket_count) {
            prefix_sum[index + 1] += 1;
        }
    }

    /**
     * Use prefixes and optimize range query efficiency.
     */
    void build()
    {
        for (int i = 1; i < prefix_sum.size(); ++i) {
            prefix_sum[i] += prefix_sum[i - 1];
        }
    }

    int getCountBetweenValues(int left, int right)
    {
        int left_index = bucket_count * ((left - min_value) * 1.0 / (max_value - min_value));
        int right_index = bucket_count * ((right - min_value) * 1.0 / (max_value - min_value));

        return prefix_sum[right_index + 1] - prefix_sum[left_index];
    }

    int getCountAtValue(int value)
    {
        int index = bucket_count * ((value - min_value) * 1.0 / (max_value - min_value));
        if (index >= 0 && index < bucket_count) {
            double bucket_width = (max_value - min_value) / static_cast<double>(bucket_count);
            return (prefix_sum[index + 1] - prefix_sum[index]) / bucket_width;
        }
        return 0;
    }
};