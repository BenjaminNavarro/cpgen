#pragma once

#include <memory>

namespace cpgen {

class ProjectManager {
public:
    ProjectManager();
    ~ProjectManager(); // = default

private:
    class pImpl;
    std::unique_ptr<pImpl> impl_;

    const pImpl& impl() const;
    pImpl& impl();
};

} // namespace cpgen