COVERALLS_REPO_TOKEN=CteyznTyrfm3aLBu5XefQ1fU9K5fqFoZ1 \
cpp-coveralls --verbose --skip-ssl-verify\
    --skip-ssl-verify \
    --exclude test/googletest \
    --exclude test/build \
    --exclude build \
    --exclude cmake-modules --skip-ssl-verify
cpp-coveralls -h
