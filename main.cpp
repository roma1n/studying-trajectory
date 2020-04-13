#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <queue>


using CourseId = size_t;

struct Course {
    CourseId id_;
    size_t level_;

    std::vector<CourseId> dependence_;

    static Course Read(CourseId id) {
        Course result;

        result.id_ = id;

        std::cout << "Enter course level" << std::endl;

        std::cin >> result.level_;

        std::cout << "Enter number of course depensences" << std::endl;

        size_t dependence_number;
        std::cin >> dependence_number;
        result.dependence_.resize(dependence_number);

        std::cout << "Enter course depensences" << std::endl;



        return result;
    };
};

struct StudentInfo {
    size_t third_level_wanted_number_;
    size_t fifth_level_wanted_number_;
    size_t second_level_total_number_;
    std::vector<size_t> second_level_priorities_;
    std::vector<CourseId> third_level_;
    std::vector<CourseId> fifth_level_;

    StudentInfo(
        size_t third_level_wanted_number,
        size_t fifth_level_wanted_number, size_t
        second_level_total_number):

        third_level_(third_level_wanted_number_, 0),
        fifth_level_(fifth_level_wanted_number_, 0),
        second_level_priorities_(second_level_total_number_) {

    }

    static StudentInfo Read() {
        size_t third_level_wanted_number = 0;
        std::cin >> third_level_wanted_number;

        size_t fifth_level_wanted_number = 0;
        std::cin >> third_level_wanted_number;

        size_t second_level_total_number = 0;
        std::cin >> third_level_wanted_number;


        StudentInfo result(third_level_wanted_number, fifth_level_wanted_number, second_level_total_number);
        /// read second level priorities

        std::cout << "Enter priorities for second level courses" << std::endl;

        for (auto& i: result.second_level_priorities_) {
            std::cin >> i;
        }

        std::cout << "Enter wanted third level courses" << std::endl;

        for (auto& i: result.third_level_) {
            std::cin >> i;
        }

        std::cout << "Enter wanted fifth level courses" << std::endl;

        for (auto& i: result.fifth_level_) {
            std::cin >> i;
        }
        return result;
    }
};

int Mark(const std::vector<CourseId>& start, const std::vector<Course>& courses, std::vector<bool>& to_study) {
    std::queue<CourseId> queue;

    int marked = 0;

    for (auto c: start) {
        if (!to_study[c]) {
            ++marked;
            to_study[c] = true;
            queue.push(c);
        }
    }

    while (!queue.empty()) {
        CourseId t = queue.front();
        for (auto dependence: courses[t].dependence_) {
            if (!to_study[dependence]) {
                ++marked;
                to_study[dependence] = true;
                queue.push(dependence);
            }
        }
    }
    return marked;
}

int Mark(CourseId course_id, const std::vector<Course>& courses, std::vector<bool>& to_study) {
    if (to_study[course_id]) {
        return 0;
    }
    std::vector<CourseId> start(1, course_id);
    return Mark(start, courses, to_study);
}

int MarkWanted(const StudentInfo& student_info, const std::vector<Course>& courses, std::vector<bool>& to_study) {
    std::vector<CourseId> start;

    for (auto c: student_info.third_level_) {
        start.push_back(c);
    }

    for (auto c: student_info.fifth_level_) {
        start.push_back(c);
    }

    return Mark(start, courses, to_study);
}

int FindBestExtraCourse(
    const std::vector<Course>& courses,
    std::vector<bool>& to_study,
    int level) {

    std::vector<bool> to_study_best = to_study;
    std::vector<bool> to_study_start = std::move(to_study);

    int result = courses.size() + 1;

    for (auto c: courses) {
        if (!to_study_start[c.id_] && c.level_ == level) {
            to_study = to_study_start;
            auto extra = Mark(c.id_, courses, to_study);
            if (extra < result) {
                result = extra;
                to_study_best = to_study;
            }
        }
    }

    to_study = std::move(to_study_best);

    return result;
}

int FindBestExtraCourses(
    const std::vector<Course>& courses,
    std::vector<bool>& to_study,
    int fifth_level,
    int fourth_level,
    int third_level) {

    if (fifth_level > 0) {
        return FindBestExtraCourse(courses, to_study, 5);
    } else if (fourth_level > 0) {
        return FindBestExtraCourse(courses, to_study, 4);
    } else if (third_level > 0) {
        return FindBestExtraCourse(courses, to_study, 3);
    } else {
        return -1;
    }
}

int GetStudiedCoursesNumber(const std::vector<bool>& to_study) {
    int result = 0;
    for (auto i: to_study) {
        if (i) {
            ++result;
        }
    }
    return result;
}

std::vector<CourseId> ProcessStudent(const StudentInfo& student_info, const std::vector<Course>& courses) {
    std::vector<bool> to_study(courses.size(), false);

    MarkWanted(student_info, courses, to_study);

    std::vector<CourseId> result;
    for (int i = 0; i < to_study.size(); ++i) {
        if (to_study[i]) {
            result.push_back(courses[i].id_);
        }
    }

    int fifth_level_left = 1;
    int fourth_level_left = 3;
    int third_level_left = 5;

    for (int i = 0; i < to_study.size(); ++i) {
        if (to_study[i]) {
            if (courses[i].level_ == 5) {
                --fifth_level_left;
            } else if (courses[i].level_ == 4) {
                --fourth_level_left;
            } else if (courses[i].level_ == 3) {
                --third_level_left;
            }
        }
    }

    int extra_greater_second_level = FindBestExtraCourses(
        courses,
        to_study,
        fifth_level_left,
        third_level_left,
        fourth_level_left);

    while (extra_greater_second_level != -1) {
        extra_greater_second_level = FindBestExtraCourses(
            courses,
            to_study,
            fifth_level_left,
            third_level_left,
            fourth_level_left);
    }

    /// last stage


    return result;
}

std::vector<Course> ReadCourses() {
    size_t n;
    std::cin >> n;

    std::vector<Course> result;
    for (size_t i = 0; i < n; ++i) {
        auto course = Course::Read(i);
        result.push_back(std::move(course));
    }
    return result;
}

void Solve() {
    auto courses = ReadCourses();
    auto student_info = StudentInfo::Read();
    auto result = ProcessStudent(student_info, courses);

    for (auto id: result) {
        std::cout << id << " ";
    }
}

int main() {
    Solve();
    return 0;
}
