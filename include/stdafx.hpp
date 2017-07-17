#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include <SOIL/SOIL.h>
#include <GL/glew.h>

#include <oglplus/all.hpp>
#include <oglplus/images/random.hpp>
#include <oglplus/images/checker.hpp>
#include <oglplus/dsa/ext/texture.hpp>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/variant.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
