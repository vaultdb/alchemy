package org.vaultdb.plan.execution.slice.statistics;

import java.util.Comparator;

// basically a pair for keeping track of where a slice key has values
public class SlicePlacement implements Comparator<SlicePlacement>, Comparable<SlicePlacement> {
	private long siteId;
	private long count;
	
	
	public SlicePlacement(long siteId2, long valueCount) {
		siteId = siteId2;
		count = valueCount;
	}
	
	public long getSiteId() {
		return siteId;
	}
	public void setSiteId(int siteId) {
		this.siteId = siteId;
	}
	public long getCount() {
		return count;
	}
	public void setCount(long count) {
		this.count = count;
	}
	
	@Override
	public boolean equals(Object o) {
		if(o instanceof SlicePlacement) {
			SlicePlacement ap = (SlicePlacement) o;
			if(ap.count == this.count && ap.siteId == this.siteId) {
				return true;
			}
		}
		
		return false;
	}
	
	public String toString() {
		return new String("(" + siteId + ", " + count + ")" );
	}

	@Override
	public int compareTo(SlicePlacement o) {
		int cmp = compareLong(siteId, o.siteId);
		if(cmp != 0) {
			return cmp;
		}
		
		return compareLong(count, o.count);
		
	}

	@Override
	public int compare(SlicePlacement o1, SlicePlacement o2) {
		return o1.compareTo(o2);
	}

	private int compareLong(long lhs, long rhs) {
		Long l = Long.valueOf(lhs);
		Long r = Long.valueOf(rhs);
		return l.compareTo(r);
	}
	
	
	
}
