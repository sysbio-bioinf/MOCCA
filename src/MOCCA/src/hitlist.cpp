#include "hitlist.h"

#include <iosfwd>
#include <iomanip>

void print_clusters(const Clusters &clusters, std::ostream &out) 
{
	Clusters::const_iterator iter;
	int ii;

	for(iter=clusters.begin(), ii=0; iter!=clusters.end(); ++iter, ++ii) {
		out << ii << " : ";
		std::copy((*iter).begin(),(*iter).end(),std::ostream_iterator<Index>(out," "));
		out << std::endl;
	}
}


void clusters_to_hits( const Clusters & c, Hits & h, unsigned int n )
{
	Clusters::size_type			i;
	HitList::const_iterator		hfirst,
								hlast;

	if( n > 0 )
	{
		h = Hits(n);
	}

	n = h.size();


	for( i = 0; i < c.size(); ++i )
	{
		hfirst = c[i].begin();
		hlast  = c[i].end();

		while( hfirst != hlast )
		{
			if( (*hfirst) >= n )
			{
				// extend array
				n = (*hfirst) + 1;
				h.resize( n );
			}
			h[ *hfirst ] = i;
			++hfirst;
		}
	}
}
