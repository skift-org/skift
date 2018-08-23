import project

class Target(object):
    """docstring for ."""
    def __init__(self, name, data, projects):
        self.name = name
        self.defines = data["defines"]
        self.projects = []

        for p in data["projects"]:
            if p in projects:
                self.projects.append(projects[p])
            else:
                print("Missing project %s for target %s!" % (p, name))


    def clean():
        for project in self.projects:
            project.clean()

    def build():
        for project in self.projects:
            projects.build(self.defines, self.projects)

    def rebuild():
        clean()
        build()
