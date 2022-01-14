
Polygon valtrAlgo(int n) {
    vector<double> xp(n), yp(n);
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<double> xdis(1.0, 839.0);
    uniform_real_distribution<double> ydis(1.0, 639.0);
    uniform_real_distribution<double> dis(0, 1);
    for (int i = 0; i < n; i++) {
        xp[i] = xdis(gen);
        yp[i] = ydis(gen);
    }
    sort(xp.begin(), xp.end());
    sort(yp.begin(), yp.end());

    double minx = xp[0], maxx = xp[n - 1], miny = yp[0], maxy = yp[n - 1];

    vector<double> xvec, yvec;

    double lastTop = minx, lastBot = minx;
    for(int i = 1; i < n - 1; i++) {
        double x = xp[i];
        if(dis(gen) > 0.5) {
            xvec.push_back(x - lastTop);
            lastTop = x;
        }
        else {
            xvec.push_back(lastBot - x);
            lastBot = x;
        }
    }

    xvec.push_back(maxx - lastTop);
    xvec.push_back(lastBot - maxx);

    double lastLeft = miny, lastRight = miny;
    for(int i = 1; i < n - 1; i++) {
        double y = yp[i];
        if(dis(gen) > 0.5) {
            yvec.push_back(y - lastLeft);
            lastLeft = y;
        }
        else {
            yvec.push_back(lastRight - y);
            lastRight = y;
        }
    }

    yvec.push_back(maxy - lastLeft);
    yvec.push_back(lastRight - maxy);

    // randomly shuffle yvec
    random_shuffle(yvec.begin(), yvec.end());
    
    vector<Point> vec(n);
    for(int i = 0; i < n; i++) {
        vec[i].x = xp[i];
        vec[i].y = yp[i];
    }

    sort(vec.begin(), vec.end(), [](const Point &a, const Point &b) {
        return atan2(a.y, a.x) < atan2(b.y, b.x);
    });

    cout << "vec\n";
    for(int i = 0; i < n; i++) {
        cout << vec[i] << " ";
    }
    cout << endl;

    double x = 0, y = 0;
    double minPolygonX = 0;
    double minPolygonY = 0;

    vector<Point> pts(n);
    for(int i = 0; i < n; i++) {
        pts[i].x = x;
        pts[i].y = y;

        x += vec[i].x;
        y += vec[i].y;
        minPolygonX = min(minPolygonX, x);
        minPolygonY = min(minPolygonY, y);
    }

    double xShift = minx - minPolygonX;
    double yShift = miny - minPolygonY;

    double mx = 0, my = 0;
    for(int i = 0; i < n; i++) {
        pts[i].x += xShift;
        pts[i].y += yShift;
        mx = max(mx, pts[i].x);
        my = max(my, pts[i].y);
    }

    for(int i = 0; i < n; i++) {
        pts[i].x = (pts[i].x / (mx + 20)) * 839;
        pts[i].y = (pts[i].y / (my + 20)) * 639;
    }

    cout << "pts\n";
    for(int i = 0; i < n; i++) {
        cout << pts[i] << " ";
    }
    cout << endl;

    return Polygon(pts);
}