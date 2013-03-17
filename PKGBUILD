# Contributor: Andre Ericson <de.ericson@gmail.com>

pkgname=python2-tinydconf
pkgver=0.2
pkgrel=1
pkgdesc="A wrapper around dconf client api"
arch=('i686' 'x86_64')
url="https://github.com/aericson/tinydconf"
license=('MIT')
depends=('python2' 'dconf')
makedepends=()
provides=()
conflicts=()
replaces=()
backup=()
source=(https://github.com/aericson/tinydconf/archive/master.zip)
md5sums=('76f875a16c6c04df7d55e45a3dd2f386')
options=(!emptydirs)

package() {
  cd "$srcdir/tinydconf-master"
  python2 setup.py install --root="$pkgdir/" --optimize=1
}
